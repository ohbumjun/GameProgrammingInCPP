#pragma once

#include <cstdint>
#include <type_traits>
#include <cassert>
#include <algorithm>
#include <unordered_map>

// https://github.com/vblanco20-1/decs


// memory page �� ũ�Ⱑ 16384 �̴�.
// memory page ��, os �� ���ؼ� �Ҵ�, ���� �� �� �ִ� ���� ���� ������ �޸��̴�.
// data chunk �� ũ�⸦ �ش� ũ��� ���� �ϸ�, ������ chunk �� single memory page ��
// �� �°� ���� �ȴ�.
// �׷���, chunk �� �Ҵ�, �����ϴ� ������ �� ȿ�������� �� �� �ְ�
// fragmentation �� ���� �� �ִ�.

// Fragmentation occurs when memory is allocated and deallocated in a way that leaves small, 
// unused gaps of memory between allocated blocks. 
// These gaps can make it difficult to allocate large blocks of memory,
constexpr size_t BLOCK_MEMORY_16K = 16384;

//forward declarations
namespace decs {
	using byte = unsigned char;
	static_assert (sizeof(byte) == 1, "size of 1 byte isnt 1 byte");
	constexpr size_t BLOCK_MEMORY_16K = 16384;
	constexpr size_t BLOCK_MEMORY_8K = 8192;

	struct Metatype;
	struct MetatypeHash;
	struct EntityID;
	struct DataChunkHeader;
	struct DataChunk;
	struct ChunkComponentList;
	struct Archetype;
	struct EntityStorage;
	struct Query;
	struct ECSWorld;
}
namespace decs {
	inline constexpr uint64_t hash_64_fnv1a(const char* key, const uint64_t len) {

		uint64_t hash = 0xcbf29ce484222325;
		uint64_t prime = 0x100000001b3;

		for (int i = 0; i < len; ++i) {
			uint8_t value = key[i];
			hash = hash ^ value;
			hash *= prime;
		}

		return hash;
	}

	inline constexpr uint64_t hash_fnv1a(const char* key) {
		uint64_t hash = 0xcbf29ce484222325;
		uint64_t prime = 0x100000001b3;

		int i = 0;
		while (key[i]) {
			uint8_t value = key[i++];
			hash = hash ^ value;
			hash *= prime;
		}

		return hash;
	}

	template<typename T, int BucketSize>
	struct HashCache {

		struct Bucket {
			std::array<T, BucketSize> items;
			std::array<size_t, BucketSize> item_hashes;
		};

		HashCache() {
			for (int i = 0; i < 8; ++i) {
				buckets[i] = Bucket{};
			}
		}

		std::array<Bucket, 8> buckets;
	};

	struct MetatypeHash {

		// Metatype::build_hash �Լ� ����
		uint64_t name_hash{ 0 };

		uint64_t matcher_hash{ 0 };

		bool operator==(const MetatypeHash& other)const {
			return name_hash == other.name_hash;
		}
		template<typename T>
		static constexpr const char* name_detail() {
			return __FUNCSIG__;
		}

		template<typename T>
		static constexpr uint64_t hash() {

			static_assert(!std::is_reference_v<T>, "dont send references to hash");
			static_assert(!std::is_const_v<T>, "dont send const to hash");
			return hash_fnv1a(name_detail<T>());
		}
	};
	struct Metatype {

		using ConstructorFn = void(void*);
		using DestructorFn = void(void*);

		MetatypeHash hash;

		const char* name{ "none" };
		ConstructorFn* constructor;
		DestructorFn* destructor;
		uint16_t size{ 0 };
		uint16_t align{ 0 };

		// ���� align �̶�� ����, 
		// type �� size..? ó�� ���δ�.
		bool is_empty() const { return align == 0; };


		template<typename T>
		static constexpr MetatypeHash build_hash() {

			using sanitized = std::remove_const_t<std::remove_reference_t<T>>;

			MetatypeHash hash;

			// T Type �� ���� ������ hash ���� ���Ѵ�.
			hash.name_hash = MetatypeHash::hash<sanitized>();

			// 63���� �������� �� ������ ���� ���Ѵ�.
			// 0 ~~ 62 ������ ���� �� ���̴�.
			uint64_t matcher	= (uint64_t)((hash.name_hash) % 63L);

			// �⺻�� 1�� �ȴ�.
			uint64_t defaultDecimal		= (uint64_t)0x1L;

			defaultDecimal = defaultDecimal << matcher;

			hash.matcher_hash |= defaultDecimal;
			
			return hash;
		};
		
		
		// ���������� meta.constructor ���� ������
		// new �� �̿��� dynamic allocation �� �����ϰ� �ֱ� ������
		template<typename T>
		// static constexpr Metatype build() {
		static Metatype build() {

			Metatype meta{};
			meta.hash = build_hash<T>();
			meta.name = MetatypeHash::name_detail<T>();

			if constexpr (std::is_empty_v<T>)
			{
				meta.align = 0;
				meta.size = 0;
			}
			else {
				meta.align = alignof(T);
				meta.size = sizeof(T);
			}

			meta.constructor = [](void* p) {
				new(p) T{};
			};
			meta.destructor = [](void* p) {
				((T*)p)->~T();
			};

			return meta;
		};
	};

	// has stable pointers, use name_hash for it
	static std::unordered_map<uint64_t, Metatype> metatype_cache;

	struct EntityID {
		uint32_t index;
		uint32_t generation;
	};
	struct DataChunkHeader {

		// pointer to the signature for this block
		// componentList ��� ����ü �ȿ�, ���� byte �޸� DataChunk �߿���
		// � offset �� � component �� �ִ����� ���� ������ ��� ������ ���δ�.
		struct ChunkComponentList* componentList;

		// �׸��� ArcheType Class �� ������ �����ϰ�
		// �ش� ArcheType �� � ���·ε� DataChunk ? ���� ��� �ִ� �� ����.
		struct Archetype* ownerArchetype{ nullptr };

		// chunk ���� ���Ḯ��Ʈ ���·� �����ϴ� ������ ���δ�.
		struct DataChunk* prev{ nullptr };
		struct DataChunk* next{ nullptr };

		// �ش� chunk �� ������ �ִ� entity ���� 
		// �ش� ũ�Ⱑ , componentList �� chunkCapacity ���� Ŭ �� ����.
		// ������, �ش� Chunk �� entity �� �߰��� ������ last++ �� ���ش�.
		int16_t last{ 0 };
	};

	// alignas(32) : struct's starting memory address must be multiple of 32
	// �� 32 ��ŭ align �� ���ߴ� ���ϱ� ?

	// This alignment requirement is necessary 
	// to ensure that the struct can be accessed efficiently by the CPU.
	// �� ?
	// cpu ��, optimal performance �� ���ؼ�
	// they prefer data to be located on specific memory addresses aligned to certain boundaries
	// ���� 1 . Cache Lines: 
	// CPUs have caches, which are smaller, faster memories 
	// that hold frequently accessed data. 
	// Data is transferred between main memory 
	// and the cache in fixed-size chunks called cache lines. 
	// If a data structure (like DataChunk) straddles multiple cache lines, 
	// accessing it requires fetching data from multiple lines, 
	// which is slower than fetching it from a single line. 
	// By aligning the struct, you ensure it fits entirely within a single cache line, 
	// enabling faster access.
	// ��. �Ʒ� storage �� �ݵ�� ù��° ������ �־� �Ѥ�.
	// DataChunk �� ���� �ּҰ�, storage �� �����ϰ� �ؾ� �Ѵ�.
	struct alignas(32) DataChunk {
		// ���� ������ ũ���, BLOCK_MEMORY_16K - Header ũ�⸸ŭ �Ѵ�.
		byte storage[BLOCK_MEMORY_16K - sizeof(DataChunkHeader)];
		DataChunkHeader header;
	};
	static_assert(sizeof(DataChunk) == BLOCK_MEMORY_16K, "chunk size isnt 16kb");

	// adv::ChunkComponentList* build_component_list �Լ��� ���ؼ� ���ο� component_list �� �����.
	struct ChunkComponentList
	{
		struct CmpPair {
			// Component �� �����Ǵ� Reflection ������� �����ϸ� �ȴ�.
			const Metatype* type;

			// �ش� Component �� ������ Hash ����
			MetatypeHash hash;
			uint32_t chunkOffset;
		};

		// � set �� ���ִ°�
		// ex) �ش� DataChunk �� Transform, Move, Name �̶�� 3���� Component Set �� ��´ٸ�
		// �̷� set �� � �� �ִ°�.
		// �ٸ� ���� �ϸ�, �ִ� ��� entity ������ �� �� �ִ°�. 
		int16_t chunkCapacity;

		// �ش� vector ��, unoredered_map ���·� �����ϴ� �͵� ���� �� ����.
		std::vector<CmpPair> components;
	};

	template<typename T>
	struct ComponentArray {

		ComponentArray() = default;
		ComponentArray(void* pointer, DataChunk* owner) {
			data = (T*)pointer;
			chunkOwner = owner;
		}

		const T& operator[](size_t index) const {
			return data[index];
		}
		T& operator[](size_t index) {
			return data[index];
		}
		bool valid()const {
			return data != nullptr;
		}
		T* begin() {
			return data;
		}
		T* end() {
			return data + chunkOwner->header.last;
		}
		int16_t size() {
			return chunkOwner->header.last;
		}
		// �̷��� data ������ ù���� ��� ������ �����ϸ�
		// ComponentArray �������� �����ּҰ� �� data �� �ȴ�.
		// �ٽ� ���ؼ�, ComponentArray* �� �Ʒ� data �� �ش��ϴ� �޸� �ּҰ� �Ǵ� ���̴�.
		// >> �׷��� data ��� �������� � ���� ���� ���ϱ� ?
		// chunk ������, component �޸� �����ּҰ� ���� �ȴ�.
		// ex) Chunk �� [Entity ID, Id..../Transform, Transform, Transform.../Name, NAme, Name...] �̷� ���°� �ȴ�.
		// �̶�, Transform ù��° �������� �����ּҰ� data ��� ������ ������ �ǰ�
		// data[entity id] �� ���ؼ�, �ش� entity id �� �����Ǵ� component data �� ���� �� �ְ� �Ǵ� ���̴�.
		// ���� ) get_entity_component �Լ�
		T* data{ nullptr };
		DataChunk* chunkOwner{ nullptr };
	};

	struct Archetype {
		ChunkComponentList* componentList;
		struct ECSWorld* ownerWorld;
		size_t componentHash;
		// (����) set_chunk_full
		// �ش� ArcheType �� ��� �ִ� DataChunk �� �߿��� full �� �� DataChunk �� ��ΰ�.
		int full_chunks;
		// �ϴ�, ArcheType �� �������� chunk �� ��� �ְ� �ϴ� �� ����.
		std::vector<DataChunk*> chunks;
	};

	// entity �� ���õ� ������ ���� class �̴�.
	struct EntityStorage {
		// �ش� entity �� ���� chunk
		DataChunk* chunk;

		// entity ��Ȱ���� ����Ͽ� generation �̶�� ������ �����ϳ�.
		// �׷��� EntityID �� generatio �̶�� ������ �ִµ�,
		// EntityStorage ���� �־�� �ϴ°ǰ� ?
		// �亯 : is_entity_valid �Լ��� ����, ���� ������ entityId.generation �� ���ƾ�����, valid �� entity �̴�.
		uint32_t generation;

		// �ش� chunk ���� ���° idex �� ��ġ�ϴ°�.
		uint16_t chunkIndex;

		bool operator==(const EntityStorage& other) const {
			return chunk == other.chunk && generation == other.generation && chunkIndex == other.chunkIndex;
		}

		bool operator!=(const EntityStorage& other) const {
			return !(other == *this);
		}
	};

	struct Query {
		// ã�� component ��� 
		std::vector<MetatypeHash> require_comps;

		// ã�� �ʴ� component ���
		std::vector<MetatypeHash> exclude_comps;

		// store the "bitwise OR of the matcher hashes" of the required and excluded components.
		uint64_t require_matcher{ 0 };
		uint64_t exclude_matcher{ 0 };


		bool built{ false };

		template<typename... C>
		Query& with() {
			require_comps.insert(require_comps.end(), { Metatype::build_hash<C>()... });

			return *this;
		}

		template<typename... C>
		Query& exclude() {
			exclude_comps.insert(exclude_comps.end(), { Metatype::build_hash<C>()... });

			return *this;
		}

		// Query �� �����ϴ� �Լ� 
		Query& build() {
			auto compare_hash = [](const MetatypeHash& A, const MetatypeHash& B) {
				return A.name_hash < B.name_hash;
			};

			auto build_matcher = [](const std::vector<MetatypeHash>& types) {
				size_t and_hash = 0;

				for (auto type : types)
				{
					and_hash |= type.matcher_hash;
				}
				return and_hash;
			};

			auto remove_eid = [](const MetatypeHash& type) {
				return (type == Metatype::build_hash<EntityID>());
			};

			// reqire_comps, exclude_comps ��Ͽ��� EntityId type ���� �������� �����.
			require_comps.erase(std::remove_if(require_comps.begin(), require_comps.end(), remove_eid), require_comps.end());
			exclude_comps.erase(std::remove_if(exclude_comps.begin(), exclude_comps.end(), remove_eid), exclude_comps.end());

			std::sort(require_comps.begin(), require_comps.end(), compare_hash);
			std::sort(exclude_comps.begin(), exclude_comps.end(), compare_hash);

			require_matcher = build_matcher(require_comps);
			exclude_matcher = build_matcher(exclude_comps);
			built = true;
			return *this;
		}
	};

	struct ECSWorld {
		std::vector<EntityStorage> entities;
		std::vector<uint32_t> deletedEntities;

		// �Ʒ��� �ڷᱸ���� �� ����� �����ص� �� �� ����.
		// ECSWorld �����ڸ� ����, nullArch �� ����� ����
		// �Ʒ� �ڷᱸ�� key �� '0' �� �����Ѵ�.
		// 1) ���⼭ '0' �̶�� ����...'0'�� set �� ����ִ� ArcheType��..�̶�� �ǹ��ΰ�?
		// 2) find_or_create_archetype �Լ��� ����, component �� hash ������ ���ļ� matcher ��� uint64_t �� �����.
		//    ��, key ��, hash ���տ� �ش��ϴ� ������ ���δ�.
		std::unordered_map<uint64_t, std::vector<Archetype*>> archetype_signature_map{};
		std::unordered_map<uint64_t, Archetype*> archetype_map{};

		// �������� archetype���� vector ���·� �����ϴ� ������ ���δ�.
		// (ECSWorld �����ڿ����� nullArche �� �����.)
		std::vector<Archetype*> archetypes;
		//unique archetype hashes
		std::vector<size_t> archetypeHashes;
		//bytemask hash for checking
		// archetype_signature_map �� key ������, �� �ٽ� vector ���·� �����Ѵ�.
		std::vector<size_t> archetypeSignatures;

		std::unordered_map<uint64_t, void*> singleton_map{};

		// ���� ������� entity ����
		int live_entities{ 0 };
		// �̹� �׾ ��Ȱ���� �� �ִ� entity ����
		int dead_entities{ 0 };
		inline ECSWorld();

		~ECSWorld()
		{
			for (Archetype* arch : archetypes)
			{
				for (DataChunk* chunk : arch->chunks) {
					delete chunk;
				}
				delete arch;
			}
		};
		//needs push_back(DataChunk*) to work, returns number
		template<typename Container>
		int gather_chunks(Query& query, Container& container);

		template<typename Func>
		void for_each(Query& query, Func&& function);

		template<typename Func>
		void for_each(Func&& function);

		/*
		template<typename C>
		void add_component(EntityID id, C& comp);
		*/

		template<typename C>
		C* add_component(EntityID id);

		template<typename C>
		void remove_component(EntityID id);

		template<typename C>
		bool has_component(EntityID id);

		template<typename C>
		C& get_component(EntityID id);

		template<typename C>
		C* set_singleton();
		template<typename C>
		C* set_singleton(C&& singleton);

		template<typename C>
		C* get_singleton();

		template<typename ... Comps>
		inline EntityID new_entity();

		inline void destroy(EntityID eid);

		// �� ó�� ECSWorld �����ڿ��� ��������
		// nullArch �� �������ִ� �Լ�
		Archetype* get_empty_archetype() { return archetypes[0]; };
	};

	namespace adv {
		//forward declarations
		inline int insert_entity_in_chunk(DataChunk* chunk, EntityID EID, bool bInitializeConstructors = true);
		inline EntityID erase_entity_in_chunk(DataChunk* chunk, uint16_t index);
		inline DataChunk* build_chunk(ChunkComponentList* cmpList);

		// type_list is a simple template that takes a variadic list of types. 
		// It's used to represent a list of types.
		// it's just a way to group together types in template meta programming
		template<typename... Type>
		struct type_list {};

		// args is a function template that takes a pointer to a member function 
		// ��, ���ڰ� Ư�� class �� ��� �Լ��� ���� �������̴�.
		// and returns a type_list of the member function's parameter types. 
		// It's used to get the parameter types of a member function.
		// ��, �ش� �Լ��� �Ķ���� Ÿ�Ե���, type_list ��� ����ü ���·� ���Ͻ����ִ� ���̴�.
		template<typename Class, typename Ret, typename... Args>
		type_list<Args...> args(Ret(Class::*)(Args...) const);

		// ���ø� ���� T �� ���� MetaType ������ ������ִ� �Լ�
		template<typename T>
		static const Metatype* get_metatype() {
			// type T �� ���� Metatype �� �� �ѹ��� ���ǵ���
			// static local ������ Ȱ���Ѵ�.
			// �Ʒ� ������, ���� �Լ��� ���� + ȣ��. ���� �Ѳ����� ���Ե� ���̴�.
			static const Metatype* mt = []() {
				constexpr size_t name_hash = Metatype::build_hash<T>().name_hash;

				auto type = metatype_cache.find(name_hash);
				if (type == metatype_cache.end()) {
					/*
					constexpr �Լ� ���� ����

					>> virtual �̸� �ȵȴ�

					>> return type �� literal type �̾�� �Ѵ�.
					��, �ٸ� ���� �ؼ� compile time �� �����Ǵ� object Ȥ�� ���̾�� �Ѵ�.
					ex) int, float ��
					����, literal type �� �ƴ� ����, runtime �� �����Ǵ� �͵��̴�.
					user defined type ��, Ư�� ������ �����ؾ߸� literal type �� �ȴ�.
					ex) std::function, std::vector ��
					ex) Ȥ�� dynamic memory allocation �� �Ͼ�� �͵鵵 literal type �� �ƴϴ�.
					ex) 
					- parameter �鵵 literal type �̾�� �Ѵ�.
					- function body �� function try block �̸� �ȵȴ�.
					ex) void foo() try {
						   // function body
					   } catch (...) {
						   // handle exceptions
						}
						�̿� ���� ���°� �Ǹ� �ȵȴٴ� ���̴�.

					>> ���������� non-constexpr �Լ��� ȣ���ϸ� �ȵȴ�.

					>> non-trivial destructor �� ����� �Ѵ�.
					�׷��ٸ� "trivial" destructor �� �����ϱ� ?

					- user �� �����ϸ� �ȵȴ�.
					- delete �Ǿ�� �ȵȴ�.
					- class �� virtual destructor �� ������ �ȵȴ�.
					- base class �鵵 trivial destructor �� ������ �Ѵ�.
					*/
					// constexpr Metatype newtype = Metatype::build<T>();
					Metatype newtype = Metatype::build<T>();
					metatype_cache[name_hash] = newtype;
				}
				return &metatype_cache[name_hash];
			}();
			return mt;
		}

		//reorder archetype with the fullness
		inline void set_chunk_full(DataChunk* chunk) {

			Archetype* arch = chunk->header.ownerArchetype;
			if (arch) {
				arch->full_chunks++;

				//do it properly later
				// �ش� Archetype �� ��� �ִ� chunk �� �� �� �ִ�
				// entity �� �ִ� ����
				int archsize = arch->componentList->chunkCapacity;

				// std::partition ��, ���ǿ� ���� element ���� �������Ѵ�.
				// ������ �����ϴ� ������ �տ�, �������� ���ϴ� ������ �ڿ� �д�.
				// ��, full �� data chunk ���� ���ʿ� ��ġ�ϱ� ���� �ڵ�� ���δ�.
				// �׸��� ���� full �� �ƴϰ�, �� �κ��� �ִ� chunk ���� ���ʿ� ��ġ�Ѵ�.
				std::partition(arch->chunks.begin(), arch->chunks.end(), [archsize](DataChunk* cnk) {
					return cnk->header.last == archsize;
				});
			}


		}
		//reorder archetype with the fullness
		inline void set_chunk_partial(DataChunk* chunk) {

			Archetype* arch = chunk->header.ownerArchetype;

			// archtype �� ��� �ִ� chunk �� ��, full �� ���ִ� chunk �� ������ - 1 ���ش�.
			arch->full_chunks--;

			//do it properly later
			int archsize = arch->componentList->chunkCapacity;

			// ���⼭�� set_chunk_full �Լ��� ����������
			// full ���θ� ��������, chunk ���� �������Ѵ�.
			std::partition(arch->chunks.begin(), arch->chunks.end(), [archsize](DataChunk* cnk) {
				return cnk->header.last == archsize;
			});
		}

		inline ChunkComponentList* build_component_list(const Metatype** types, size_t count) {
			// component �� total size �� ����ϰ��� �Ѵ�.
			// 1) EntityID 
			// 2) component ���� * count
			// ���⼭ count ��, type �� �����̴�.
			
			ChunkComponentList* list = new ChunkComponentList();

			// chunk �ȿ�, component data ���� ���� ���̴�.
			// �ٸ�, chunk ���ʿ��� entity id �������� ���� ������ ���̴�.
			// ��, ���� ���, MoveComponet �� size �� 8�̾��ٸ�,
			// entity id �������� �����ؼ�, �ϳ��� moveComponent �����͸� chunk �� �����ϴ� ���̴�.
			// ex) entity id ũ�� : 2 
			// move component ������ : 2 + 8 == 10
			int compsize = sizeof(EntityID);

			// ��. DataChunk ��� �� ��ü��, ���� Component Data ����� ������ �� �ִ� �༮�̴�.
			// ex) �ᱹ ���� ���, Transform, Move, Name Component �� �ϳ��� Data Chunk �ȿ�
			// ���� �� �ִٴ� ���̴�.
			for (size_t i = 0; i < count; i++) {

				compsize += types[i]->size;
			}

			size_t availibleStorage = sizeof(DataChunk::storage);
			
			{
				// Debug
				size_t headersize = sizeof(DataChunkHeader);
				bool h = true;
			}

			// 2 less than the real count to account for sizes and give some slack
			// chenk storage �� fit �� �� �ְԲ�.
			// ex) �� size : 100, compSize : 10, itemCount : 8
			// ���⼭ itemCount ��, �ش� DataChunk ��
			// �� (Transform, Move, Name Component) set �� 8�� ���ٴ� �ǹ��̴�.
			size_t itemCount = (availibleStorage / compsize) - 2;

			// chunk storage ����, ù��° component �� offset �� ����Ѵ�.
			// ���� offset ��, DataChunkHeader Size + total size of EntityID of all items
			// (����) �׷��� ���� DataChunkHeader ��ŭ�� �� �ִ� ������ ���� ?
			// �̹� DataChunk  ��� Class ����, DataChunkHeader ������ ���� �ִµ�...
			uint32_t offsets = sizeof(DataChunkHeader);

			// insert_entity_in_chunk �Լ� ����
			// DataChunk ������ : [entity ID, entity ID...][Transform, Transform..][Move, Move...][Name, Name...]
			// �̷��� ���ʿ��� entity ID, �׸��� �ڿ� Component �������� ��Ƶδ� ������� �����ϴ� ������ ���δ�.
			offsets += sizeof(EntityID) * itemCount;

			for (size_t i = 0; i < count; i++) {

				// ��� meta type ������ ��ȸ�Ѵ�.
				const Metatype* type = types[i];

				// ���� Metatype �� alignment �� �䱸�Ѵٸ�
				// �� align �� �����Ͽ� offset �� �����ش�.
				if (type->align != 0) {
					//align properly
					// ex) offset : 100, align : 8 -> remainder : 4
					// oset : 8 - 4 = 4
					// offset : 100 + 4 == 104
					size_t remainder = offsets % type->align;
					size_t oset = type->align - remainder;
					offsets += oset;
				}

				// �׸��� ���� ������ CmpPair ��� ������ �����ͷ� ��Ƽ�
				// components ��� vector �� push �Ѵ�.
				// (����) DataChunk �� byte memory, ���⼭ �� component �� ���� ��ġ��
				// ���� CmpPair ��� ������ ��Ƽ� �����ϴ� ������ ���δ�.
				// ex) Transform, Name, Move �� �ִٰ� �Ѵٸ�
				// Chunk ���� [Transform, Transform...][Name, Name...][Move, Move...] ���� ���·� �����ϰڴٴ� �ǹ��̴�.
				list->components.push_back({ type,type->hash,offsets });

				// ���������� align �� �䱸�ϸ�
				// itemCount ���� * size ��ŭ offset �� ���������ش�.
				// �̸� ����, ���� type �� data ���� DataChunk �� ���� �ϴ� ������ ���δ�.
				if (type->align != 0) {
					offsets += type->size * (itemCount);
				}

			}

			// implement proper size handling later
			// offset �� ����ũ�� ? DataChunk �� ũ�⸦ ������ �ȵȴ�. 
			assert(offsets <= BLOCK_MEMORY_16K);

			list->chunkCapacity = itemCount;

			return list;
		}

		inline size_t build_signature(const Metatype** types, size_t count) {
			size_t and_hash = 0;
			//for (auto m : types)
			for (int i = 0; i < count; i++)
			{
				//consider if the fancy hash is needed, there is a big slowdown
				//size_t keyhash = ash_64_fnv1a(&types[i]->name_hash, sizeof(size_t));
				//size_t keyhash = types[i]->name_hash;

				and_hash |= types[i]->hash.matcher_hash;
				//and_hash |=(uint64_t)0x1L << (uint64_t)((types[i]->name_hash) % 63L);
			}
			return and_hash;
		}

		inline DataChunk* create_chunk_for_archetype(Archetype* arch) {
			// ���ο� chunk �� �ϴ� �����.
			DataChunk* chunk = build_chunk(arch->componentList);

			chunk->header.ownerArchetype = arch;

			arch->chunks.push_back(chunk);

			return chunk;
		}
		inline void delete_chunk_from_archetype(DataChunk* chunk) {
			Archetype* owner = chunk->header.ownerArchetype;
			DataChunk* backChunk = owner->chunks.back();

			// ���� ������� �ϴ� chunk ��, �� ������ chunk �� �ƴ϶��
			if (backChunk != chunk) {
				for (int i = 0; i < owner->chunks.size(); i++) {
					if (owner->chunks[i] == chunk) {
						// ���� ����� chunk ��ġ��, ���� ������ chunk �� �����Ѵ�.
						owner->chunks[i] = backChunk;
					}
				}
			}
			// �׸��� ������ chunk * ������ �����ش�.
			owner->chunks.pop_back();

			// ���� chunk data �� �޸� ���� �Ѵ�.
			delete chunk;

		}
		inline bool compare_metatypes(const Metatype* A, const Metatype* B) {
			//return A->name_hash < B->name_hash;
			return A < B;
		}
		inline size_t join_metatypes(const Metatype* ATypes[], size_t Acount, const Metatype* BTypes[], size_t Bcount, const Metatype* output[]) {

			const Metatype** AEnd = ATypes + Acount;
			const Metatype** BEnd = BTypes + Bcount;

			const Metatype** A = ATypes;
			const Metatype** B = BTypes;
			const Metatype** C = output;

			while (true)
			{
				if (A == AEnd) {
					std::copy(B, BEnd, C);
				}
				if (B == BEnd) {
					std::copy(A, AEnd, C);
				}

				if (*A < *B) { *C = *A; ++A; }
				else if (*B < *A) { *C = *B; ++B; }
				else { *C = *A; ++A; ++B; }
				++C;
			}

			return C - output;
		}

		inline void sort_metatypes(const Metatype** types, size_t count) {
			std::sort(types, types + count, [](const Metatype* A, const Metatype* B) {
				return compare_metatypes(A, B);
			});
		}
		inline bool is_sorted(const Metatype** types, size_t count) {
			for (int i = 0; i < count - 1; i++) {
				if (types[i] > types[i + 1]) {
					return false;
				}
			}
			return true;
		}

		// types : �ش� Archetype �� �����ϰ� �ִ� component type ��ϵ�
		inline Archetype* find_or_create_archetype(ECSWorld* world, const Metatype** types, size_t count) {
			const Metatype* temporalMetatypeArray[32];

			// �ϳ��� component �� 32 �� �̻��� ��� ���� �ʵ��� �Ѵ�.
			assert(count < 32);

			const Metatype** typelist;

			if (false) {//!is_sorted(types, count)) {
				for (int i = 0; i < count; i++) {
					temporalMetatypeArray[i] = types[i];

				}
				sort_metatypes(temporalMetatypeArray, count);
				typelist = temporalMetatypeArray;
			}
			else {
				typelist = types;
			}

			// MetaType �� matcher_hash ���� | �����ڸ� ����, ��ģ��
			// �̸� ���� matcher �� ������.
			const uint64_t matcher = build_signature(typelist, count);

			//try in the hashmap
			auto iter = world->archetype_signature_map.find(matcher);

			// ���� ECSWorld.archetype_signature_map �� �ش� matcher �� �� Archetype ����� ã�ƿ´�.
			// ��, ���� matcher �� ���� archetype ���� ������ �� �մٴ� �ǹ��̴�.
			if (iter != world->archetype_signature_map.end()) {

				auto& archvec = iter->second;//world->archetype_signature_map[matcher];
				for (int i = 0; i < archvec.size(); i++) {

					auto componentList = archvec[i]->componentList;
					int ccount = componentList->components.size();
					if (ccount == count) {
						for (int j = 0; j < ccount; j++) {

							if (componentList->components[j].type != typelist[j])
							{
								//mismatch, inmediately continue
								goto contA;
							}
						}

						//everything matched. Found. Return inmediately
						return archvec[i];
					}

				contA:;
				}
			}

			// ������ �����ϴ� Archetype �� ���� ������, ���ο� Archetype �� �����.
			//not found, create a new one
			Archetype* newArch = new Archetype();

			newArch->full_chunks = 0;
			newArch->componentList = build_component_list(typelist, count);
			newArch->componentHash = matcher;
			newArch->ownerWorld = world;
			world->archetypes.push_back(newArch);
			world->archetypeSignatures.push_back(matcher);
			world->archetype_signature_map[matcher].push_back(newArch);

			//we want archs to allways have 1 chunk at least, create initial
			create_chunk_for_archetype(newArch);

			return newArch;
		}

		inline EntityID allocate_entity(ECSWorld* world) {
			// �ϴ� ���ο� Entity ID �� �����.
			EntityID newID;
			if (world->dead_entities == 0) {
				// ��Ȱ���� entity �� �ϳ��� ���ٸ�
				int index = world->entities.size();

				// �ƿ� ���Ӱ� entity ������ �����.
				EntityStorage newStorage;
				newStorage.chunk = nullptr;
				newStorage.chunkIndex = 0;
				newStorage.generation = 1;

				world->entities.push_back(newStorage);

				newID.generation = 1;
				newID.index = index;
			}
			else {
				// entity �� ��Ȱ���� ���̴�.
				int index = world->deletedEntities.back();
				world->deletedEntities.pop_back();

				// �ϴ� EntityStorage �� generation ������ ++ ���ش�.
				world->entities[index].generation++;

				// �� ������ EntityID ���� �������ش�.
				newID.generation = world->entities[index].generation;
				newID.index = index;
				world->dead_entities--;
			}


			world->live_entities++;
			return newID;
		}

		inline bool is_entity_valid(ECSWorld* world, EntityID id) {
			//index check
			if (world->entities.size() > id.index && id.index >= 0) {

				//generation check
				if (id.generation != 0 && world->entities[id.index].generation == id.generation)
				{
					return true;
				}
			}
			return false;
		}
		inline void deallocate_entity(ECSWorld* world, EntityID id) {

			//todo add valid check
			world->deletedEntities.push_back(id.index);
			world->entities[id.index].generation++;
			world->entities[id.index].chunk = nullptr;
			world->entities[id.index].chunkIndex = 0;

			world->live_entities--;
			world->dead_entities++;
		}

		inline void destroy_entity(ECSWorld* world, EntityID id) {
			assert(is_entity_valid(world, id));
			// chunk ���� �ش� entity �� �����Ǵ� component �� �Ҹ��� ȣ���Ͽ�, ���� memory �� �����ش�.
			erase_entity_in_chunk(world->entities[id.index].chunk, world->entities[id.index].chunkIndex);
			// ECSWorld ����, live, dead entity ���� ������ update ���ش�.
			deallocate_entity(world, id);
		}
		inline DataChunk* find_free_chunk(Archetype* arch) {
			DataChunk* targetChunk = nullptr;

			// ���� archetype �� chunk �� �ϳ��� ���ٸ�
			// chunk �� ���� �߰����ش�.
			if (arch->chunks.size() == 0) {
				targetChunk = create_chunk_for_archetype(arch);
			}
			else {
				// �ϴ� ���� �ڿ� �ִ� chunk �� �����´�.
				// �ֳ��ϸ� �츮�� set_chunk_full �Լ� ���� ���� �Ź�
				// full �� chunk ��, ��������, full �� �ƴ� chunk �� �������� ��ġ�ϱ� �����̴�.
				targetChunk = arch->chunks[arch->chunks.size() - 1];

				// chunk is full, create a new one
				// ���� ���ʿ� �ִ� ��� ������ full �̶��
				// �ƿ� ���ο� chunk �� ������ �Ѵ�.
				if (targetChunk->header.last == arch->componentList->chunkCapacity) {
					targetChunk = create_chunk_for_archetype(arch);
				}
			}
			return targetChunk;
		}

		inline void move_entity_to_archetype(Archetype* newarch, EntityID id, bool bInitializeConstructors = true) {

			//insert into new chunk
			// �ش� entity �� ���� ������ chunk ����
			DataChunk* oldChunk = newarch->ownerWorld->entities[id.index].chunk;
			// ���� ���ο� archetype �� �� ���ο� chunk ����
			DataChunk* newChunk = find_free_chunk(newarch);

			// �ϴ� ���ο� chunk �� �ش� entity �� �ִ´�.
			int newindex = insert_entity_in_chunk(newChunk, id, bInitializeConstructors);

			// entity �� ������ ���ߴ� chunk ����, ���° idx �� �Ҿ��°�.
			int oldindex = newarch->ownerWorld->entities[id.index].chunkIndex;

			// ���� component ��� ����
			int oldNcomps = oldChunk->header.componentList->components.size();
			// ���ο� component ��� ����
			int newNcomps = newChunk->header.componentList->components.size();

			auto& oldClist = oldChunk->header.componentList;
			auto& newClist = newChunk->header.componentList;

			// copy all data from old chunk into new chunk
			// bad iteration, fix later
			// �� Component Type ���� Merge struct ������ ���õ� ���̴�.
			struct Merge {
				// �ش� type �� size �� ���ΰ�
				int msize;
				// ���� component type �� ������ chunk �� ���� type �� �߿���, ���° type �̾��°�?
				int idxOld;
				// ���� component type �� ���ο� chunk �� ���� type �� �߿���, ���° type �ΰ�.?
				int idxNew;
			};

			int mergcount = 0;

			// �ϳ��� chunk �� ���� �� �ִ� �ִ� component ������ 31�� �̴�.
			Merge mergarray[32];

			// ��. ���� ���� old chunk �����͵��� ���ο� chunk ������ �Űܾ� �Ѵ�.
			// ���� old chunk ���� �ִ� type ���� ��ȸ�Ѵ�.
			for (int i = 0; i < oldNcomps; i++) {
				const Metatype* mtCp1 = oldClist->components[i].type;
				// �ش� type �� size �� �����Ѵٸ�
				// ���� ��ȿ�� type ? ���� �ƴ����� ��Ÿ���� ���� ó�� ���̱⵵ �Ѵ�.
				if (!mtCp1->is_empty()) {
					for (int j = 0; j < newNcomps; j++) {
						const Metatype* mtCp2 = newClist->components[j].type;

						// pointers are stable
						// newChunk ���� ���� type �� ã�´�.
						if (mtCp2 == mtCp1) {
							mergarray[mergcount].idxNew = j;
							mergarray[mergcount].idxOld = i;
							mergarray[mergcount].msize = mtCp1->size;
							mergcount++;
							// break ; -> �̹� ã�����ϱ� ���⼭ break �ص� ���� ������ ?
						}
					}
				}
			}

			// �� component ������ ���鼭, ���� chunk ����
			// ���ο� chunk �� �����͸� "����" ���ش�.
			// (����) ���⼭ �׳� memcpy �� �ƴ϶�
			// move �� ���ָ� �ȵǴ� �ǰ� ? ������ old chunk ������ �ش� component type ������ ��������� �����ٵ�
			for (int i = 0; i < mergcount; i++) {
				//const Metatype* mtCp1 = mergarray[i].mtype;

				//pointer for old location in old chunk
				void* ptrOld = (void*)((byte*)oldChunk + oldClist->components[mergarray[i].idxOld].chunkOffset + (mergarray[i].msize * oldindex));

				//pointer for new location in new chunk
				void* ptrNew = (void*)((byte*)newChunk + newClist->components[mergarray[i].idxNew].chunkOffset + (mergarray[i].msize * newindex));

				//memcopy component data from old to new
				memcpy(ptrNew, ptrOld, mergarray[i].msize);
			}

			//delete entity from old chunk
			erase_entity_in_chunk(oldChunk, oldindex);

			//assign entity chunk data
			newarch->ownerWorld->entities[id.index].chunk = newChunk;
			newarch->ownerWorld->entities[id.index].chunkIndex = newindex;
		}
		inline void set_entity_archetype(Archetype* arch, EntityID id) {

			// if chunk is null, we are a empty entity
			// ���� ���� �ش� entity �� �Ҵ�� chunk �� ���ٸ�
			// Archetype �� ������ �ִ� chunk �߿���, �� �κп� Entity Data �� �Ҵ������ �Ѵ�.
			if (arch->ownerWorld->entities[id.index].chunk == nullptr) {

				DataChunk* targetChunk = find_free_chunk(arch);

				int index = insert_entity_in_chunk(targetChunk, id);
				arch->ownerWorld->entities[id.index].chunk = targetChunk;
				arch->ownerWorld->entities[id.index].chunkIndex = index;
			}
			else {
				move_entity_to_archetype(arch, id, false);
			}
		}
		inline EntityID create_entity_with_archetype(Archetype* arch) {
			ECSWorld* world = arch->ownerWorld;

			// ���ο� Entity �� ����ų� ,Ȥ�� entity �� ��Ȱ���Ѵ�.
			EntityID newID = allocate_entity(world);

			set_entity_archetype(arch, newID);

			return newID;
		}
		inline Archetype* get_entity_archetype(ECSWorld* world, EntityID id)
		{
			assert(is_entity_valid(world, id));

			return world->entities[id.index].chunk->header.ownerArchetype;
		}

		template<typename C>
		bool has_component(ECSWorld* world, EntityID id);

		template<typename C>
		C& get_entity_component(ECSWorld* world, EntityID id);

		template<typename C>
		void remove_component_from_entity(ECSWorld* world, EntityID id);

		template<typename F>
		void iterate_matching_archetypes(ECSWorld* world, const Query& query, F&& function) {

			// ��� archetype ��ϵ��� ��ȸ�� ���̴�.
			// archetypeSignatures ����, archetype ���� hash key ? ����
			// vector ���·� ���ִ�.
			// �ش� ������ ��ȸ�Ѵٴ� ����, ��� world �� �����ϴ� ��� archetype ��
			// ��ȸ�ϰڴٴ� �ǹ��̴�.
			for (int i = 0; i < world->archetypeSignatures.size(); i++)
			{
				//if there is a good match, doing an and not be 0
				// require_matcher �� exclude_matcher �� �̿��Ͽ� & bit ������ �����Ѵ�.
				//if there is a good match, doing an and not be 0
				uint64_t includeTest = world->archetypeSignatures[i] & query.require_matcher;

				//implement later
				uint64_t excludeTest = world->archetypeSignatures[i] & query.exclude_matcher;

				// includeTest �� 0�� �ƴ϶�� �ǹ̴�
				// ex) world->archetypeSignatures[i] : 0011
				// ex) query.require_matcher         : 0001
				//                    result         : 0001
				// ���� archetype ��, ���� ã���� �ϴ� component type �� �ּ� �Ѱ� �̻� 
				// ������ �ִٴ� �ǹ��̴�.
				// ���� 0 �̶��, ���� ã���� �ϴ� component type �� �ϳ��� ������ ����
				// �ʴٴ� �ǹ��̴�.
				if (includeTest != 0) {

					auto componentList = world->archetypes[i]->componentList;

					//might match an excluded component, check here
					// ���� excludeTest �� 0 �� �ƴ϶��, 
					// �ش� archetype ��. ���� ã�� �ʴ� component ��ϵ� ������ �ִٴ� �ǹ��̴�
					// (����) �׷��� exCludeTest �� 0�� �ƴϸ�. �׳� ���ʿ� �ٷ�
					// continue ��Ű�� �Ǵ� �� �ƴѰ� ?
					if (excludeTest != 0) {

						bool invalid = false;

						//dumb algo, optimize later		
						// �׷���, ���� ���� �����ϰ��� �ϴ� component type �����
						// ��� ��ȸ�Ѵ�.					
						for (int mtA = 0; mtA < query.exclude_comps.size(); mtA++) {

							// �׸���, archetype �� ������ �ִ� component type ��ϵ鵵 ��ȸ�� �Ѵ�.
							for (auto cmp : componentList->components) {

								// ����, archetype ��, ���� ���� �����ϰ��� �ϴ� component type ��
								// ���ϰ� �ִٸ� break �� ���̴�.
								if (cmp.type->hash == query.exclude_comps[mtA]) {
									//any check and we out
									invalid = true;
									break;
								}
							}
							if (invalid) {
								break;
							}
						}

						// ���� archetype �� �ƴ϶��, �� ���� archetype ����� ����.
						if (invalid) {
							continue;
						}
					}

					//dumb algo, optimize later
					int matches = 0;

					// require_comp �� archetype component type �� �� ���ϸ鼭
					// match count ������ ����.
					for (int mtA = 0; mtA < query.require_comps.size(); mtA++) {

						for (auto cmp : componentList->components) {

							if (cmp.type->hash == query.require_comps[mtA]) {
								matches++;
								break;
							}
						}
					}

					// all perfect.
					// ���� ���� �Դٴ� ����, ���� ���ϴ� component �� ����
					// archetype �� ã�Ҵٴ� ���̴�.
					// �׷��� �ش� archetype �� ����, function �� �������ش�.
					if (matches == query.require_comps.size()) {

						function(world->archetypes[i]);
					}
				}


			}

		}

		template<typename C>
		C& get_entity_component(ECSWorld* world, EntityID id)
		{

			EntityStorage& storage = world->entities[id.index];

			auto acrray = get_chunk_array<C>(storage.chunk);
			assert(acrray.chunkOwner != nullptr);
			return acrray[storage.chunkIndex];
		}

		template<typename C>
		bool has_component(ECSWorld* world, EntityID id)
		{
			EntityStorage& storage = world->entities[id.index];

			auto acrray = get_chunk_array<C>(storage.chunk);
			return acrray.chunkOwner != nullptr;
		}

		// �ڼ��� ����, ���� �ش� �Լ��� entity �� archetype ������ �������� ��
		// component �����͸� chunk �� �߰��ϰų� �ϴ� �۾��� ����.
		template<typename C>
		C* add_component_to_entity(ECSWorld* world, EntityID id)
		{
			const Metatype* temporalMetatypeArray[32];

			const Metatype* type = get_metatype<C>();

			// ������ �ش� entity �� �����ִ� Archetype ������ �����´�.
			Archetype* oldarch = get_entity_archetype(world, id);

			// ���� archetype �� ��� �ִ� component ������ ��� ������ �����´�.
			ChunkComponentList* oldlist = oldarch->componentList;

			bool typeFound = false;

			int lenght = oldlist->components.size();

			// ���� Archetype ��� �߿���, ���� Add�ϰ��� �ϴ� component type �� �ִ��� �˻��Ѵ�.
			for (int i = 0; i < oldlist->components.size(); i++) {
				temporalMetatypeArray[i] = oldlist->components[i].type;

				//the pointers for metatypes are allways fully stable
				if (temporalMetatypeArray[i] == type) {
					typeFound = true;
				}
			}

			Archetype* newArch = oldarch;

			// ���� ���� ���� �߰��ϰ��� �ϴ� component type ��
			// ���� archetype �� �������� �ʾҴٸ�
			// �̴� �ش� entity �� ���� archetype ������ ��������� �Ѵٴ� ���̴�.
			if (!typeFound) {

				temporalMetatypeArray[lenght] = type;

				sort_metatypes(temporalMetatypeArray, lenght + 1);

				lenght++;

				// ���ο� component �� �߰��� ��, �ش� type ���� �ٷ�� Archetype �� ã�ų� ���� �����.
				newArch = find_or_create_archetype(world, temporalMetatypeArray, lenght);

				// �ش� �Լ��� ���ؼ�, entity �� ���ο� archetype �� �߰���Ű��
				// ���� archetype ���� �������ش�.
				set_entity_archetype(newArch, id);
			}

			if (!type->is_empty()) {

				DataChunk* entityChunk		= world->entities[id.index].chunk;
				int entityIdxInChunk				= newArch->ownerWorld->entities[id.index].chunkIndex;
				ChunkComponentList* cmpList = entityChunk->header.componentList;

				for (auto& cmp : cmpList->components) {

					const Metatype* compareType = cmp.type;

					if (cmp.hash == type->hash)
					{
						void* ptr = (void*)((byte*)entityChunk + cmp.chunkOffset + (type->size * entityIdxInChunk));

						type->constructor(ptr);

						return (C*)ptr;
					}
				}
			}

			return nullptr;
		}

		/*
		�߿� : �ϴ� �ּ�ó���Ѵ�. �̷� ������ �ܺη� comp �� �־��ִ� ��� ����
		���������� comp �� �����ϴ� ����� ���ϰ��� �Ѵ�.
		template<typename C>
		void add_component_to_entity(ECSWorld* world, EntityID id, C& comp)
		{
			const Metatype* type = get_metatype<C>();

			add_component_to_entity<C>(world, id);

			//optimize later
			if (!type->is_empty()) {

				// �ش� �ڵ带 ���� ���� component ������ chunk �� �߰����ش�.
				get_entity_component<C>(world, id) = comp;
			}
		}
		*/

		template<typename C>
		void remove_component_from_entity(ECSWorld* world, EntityID id)
		{
			const Metatype* temporalMetatypeArray[32];

			// �׳� ���� reflection type ������ ������ ������ ���δ�.
			const Metatype* type = get_metatype<C>();

			// �ش� Entity Id �� �����Ǵ� EntityStorage �� �����Ͽ�, ���� Entity �� ����
			// Archetype �� �����´�. 
			Archetype* oldarch = get_entity_archetype(world, id);
			ChunkComponentList* oldlist = oldarch->componentList;
			bool typeFound = false;

			// �ش� archetype �� ��� �ִ� component ����� ����
			int lenght = oldlist->components.size();

			for (int i = 0; i < lenght; i++) {
				temporalMetatypeArray[i] = oldlist->components[i].type;
				// ���� Archetype �� ������ �ִ� component ��� �߿���
				// ������� �ϴ� type �� ã�Ҵٸ�. typeFound ������ true �� ����
				if (temporalMetatypeArray[i] == type) {

					typeFound = true;

					//swap last
					// �׸��� ���� ã�� type ��ġ��, ������ type ��ġ ������ �����Ѵ�.
					temporalMetatypeArray[i] = oldlist->components[lenght - 1].type;
				}
			}

			Archetype* newArch = oldarch;

			// ���� ������� �ϴ� component type ��
			// archetype �ȿ��� ã�Ҵٸ�
			if (typeFound) {

				lenght--;

				// hash �� ��������, type �������� �������Ѵ�.
				sort_metatypes(temporalMetatypeArray, lenght);

				// ���ο� component ���� ? �� �����Ǵ� Archetype �� ã�´�.
				newArch = find_or_create_archetype(world, temporalMetatypeArray, lenght);

				// ���ο� ? Ȥ�� ������ �����ϴ� archetype ��, entity �� �־��ش�.
				set_entity_archetype(newArch, id);
			}
		}

		//by skypjack
		// ���� 1) chunk pointer
		// ���� 2) ȣ���ϰ��� �ϴ� �Լ��� universal reference ���·� �Ѱ��ش�.
		// - �̸� ���� r value, l value ���� ��θ� ���ڷ� ���� �� �ִ�.
		template<typename... Args, typename Func>
		void entity_chunk_iterate(DataChunk* chnk, Func&& function) {
			// std::make_tuple �� ���ؼ�, componentArray ���� ���� ���� tuple �� �����.
			auto tup = std::make_tuple(get_chunk_array<Args>(chnk)...);
#ifndef NDEBUG
			// �� Component Array �� Chunk ��, ���� Chunk �� ���������� �˻��Ѵ�.
			// (assert(std::get<decltype(get_chunk_array<Args>(chnk))>(tup).chunkOwner == chnk), ...);
#endif
			// �ش� Chunk �� ���� entity �����͸� �ڿ������� ������ ��ȸ�Ѵ�.
			for (int i = chnk->header.last - 1; i >= 0; i--) {
				// �ڿ� entity ����, �տ� entity ���� ���鼭
				// �ش� entity �� ���� ��� component ���� ���� �����Ͽ�
				// function �� �����Ѵ�.
				function(std::get<decltype(get_chunk_array<Args>(chnk))>(tup)[i]...);
			}
		}

		template<typename ...Args, typename Func>
		void unpack_chunk(type_list<Args...> types, DataChunk* chunk, Func&& function) {
			entity_chunk_iterate<Args...>(chunk, function);
		}
		template<typename ...Args>
		Query& unpack_querywith(type_list<Args...> types, Query& query) {
			return query.with<Args...>();
		}

		inline int insert_entity_in_chunk(DataChunk* chunk, EntityID EID, bool bInitializeConstructors) {
			int index = -1;

			ChunkComponentList* cmpList = chunk->header.componentList;

			// ���� �ش� chunk �� ���� entity �� ���� ������ �����Ѵٸ�
			if (chunk->header.last < cmpList->chunkCapacity) {

				index = chunk->header.last;

				// �ش� chunk �� ����ִ� entity ���� ������ +1 ���ش�.
				chunk->header.last++;

				if (bInitializeConstructors) {
					//initialize component
					for (auto& cmp : cmpList->components) {
						const Metatype* mtype = cmp.type;

						if (!mtype->is_empty()) {
							void* ptr = (void*)((byte*)chunk + cmp.chunkOffset + (mtype->size * index));

							mtype->constructor(ptr);
						}
					}
				}


				//insert eid
				EntityID* eidptr = ((EntityID*)chunk);
				eidptr[index] = EID;

				//if full, reorder it on archetype
				// ���� ���� chunk �� full �̶��, �ش� chunk �� ��� �ִ� archetype �� chunk ����
				// full ���ο� ���� �������Ѵ�.
				if (chunk->header.last == cmpList->chunkCapacity) {
					set_chunk_full(chunk);
				}
			}

			return index;
		}

		//returns ID of the moved entity
		inline EntityID erase_entity_in_chunk(DataChunk* chunk, uint16_t index) {

			ChunkComponentList* cmpList = chunk->header.componentList;

			bool bWasFull = chunk->header.last == cmpList->chunkCapacity;
			assert(chunk->header.last > index);

			// ������ chunk �� ����ִ� entity ������ 1����ũ��, ���� ����� entity �� ������ entity �� �ƴ� ���
			bool bPop = chunk->header.last > 1 && index != (chunk->header.last - 1);

			// popIndex �� �� ������ idx �� �ǹ��Ѵ�.
			int popIndex = chunk->header.last - 1;

			// entity ������ �����ش�.
			// �ش� chunk �� ��� �ִ� entity ������ �ٿ��ִ� �����̴�.
			chunk->header.last--;

			// clear and pop last
			// �� component �������� ��ȸ�Ѵ�.
			for (auto& cmp : cmpList->components) {
				const Metatype* mtype = cmp.type;

				// �ش� entity �� �����Ǵ� component �޸𸮿� �����Ѵ�.
				if (!mtype->is_empty()) {
					void* ptr = (void*)((byte*)chunk + cmp.chunkOffset + (mtype->size * index));

					// �Ҹ��ڸ� ȣ���Ѵ�.
					mtype->destructor(ptr);

					if (bPop) {
						void* ptrPop = (void*)((byte*)chunk + cmp.chunkOffset + (mtype->size * popIndex));

						// �� �������� �ִ� ������ ������, ���� ���� �޸� ��ġ�� �������ش�.
						memcpy(ptr, ptrPop, mtype->size);
					}
				}
			}

			// ���� ���� ���� entity �� ���� �޸𸮵� ������ ? �ʱⰪ? ���� �������ش�.
			EntityID* eidptr = ((EntityID*)chunk);
			eidptr[index] = EntityID{};


			// ���� chunk �� �����Ͱ� ���̻� ������� �ʴٸ�
			// �ش� chunk �� ��� �ִ� archetype ����, �ش� chunk ������ �����.
			if (chunk->header.last == 0) {
				delete_chunk_from_archetype(chunk);
			}
			else if (bWasFull) {
				set_chunk_partial(chunk);
			}

			if (bPop) {
				// ���� �߰� entity �� ���� ���̶��, �ϴ� entity Storage ����, �ش� entity �� ���õ� ������ update �Ѵ�.
				// ���� ����, ���� ������ entity ������, ���� ����� entity ���� ��ġ�� �ű�� �����̴�.
				chunk->header.ownerArchetype->ownerWorld->entities[eidptr[popIndex].index].chunkIndex = index;
				// �׸��� ���� ���� entity ��ġ��, ���� �������� ��ġ�ߴ� entity ������ �����Ѵ�.
				eidptr[index] = eidptr[popIndex];

				return eidptr[index];
			}
			else {
				// �װ� �ƴ϶�� �׳� empty entity ID �� �����Ѵ�.
				return EntityID{};
			}
		}

		inline DataChunk* build_chunk(ChunkComponentList* cmpList) {

			// ���ο� DataChunk �� �����.
			// �ش� DataChunk ���� �޸� Ǯ ���·� �����ϴ� �͵� ����ȭ�� ����� �� �� ���� �� ����.
			DataChunk* chunk = new DataChunk();
			chunk->header.last = 0;
			chunk->header.componentList = cmpList;

			return chunk;
		}
	}

	template<typename C>
	struct CachedRef
	{
		C* get_from(ECSWorld* world, EntityID target);

		C* pointer;
		EntityStorage storage;
	};



	template<typename C>
	C* CachedRef<C>::get_from(ECSWorld* world, EntityID target)
	{
		if (world->entities[target.index] != storage) {
			pointer = &world->get_component<C>(target);
			storage = world->entities[target.index];
		}
		return pointer;
	}

	template<typename T>
	inline auto get_chunk_array(DataChunk* chunk) {

		using ActualT = ::std::remove_reference_t<T>;

		// ���� ���ڷ� ���� T �� EntityID type �̶��
		if constexpr (std::is_same<ActualT, EntityID>::value)
		{
			// chunk ���� �ּҸ� EntityID �� �����Ѵ�.
			EntityID* ptr = ((EntityID*)chunk);
			return ComponentArray<EntityID>(ptr, chunk);
		}
		else {
			// �׿��� type �̶��
			constexpr MetatypeHash hash = Metatype::build_hash<ActualT>();

			// component ����� ���鼭
			// ���ڷ� ���� type �� �ش��ϴ� component �� �ִ��� �˻��Ѵ�.
			for (auto cmp : chunk->header.componentList->components)
			{
				if (cmp.hash == hash)
				{
					// ���� ���� ���ڷ� ���� type �� �ش��ϴ� component ��
					// chunk ���� ã�Ҵٸ�
					// �ش� chunk ������, �ش� component �� �޸� �������� ptr �� �����Ѵ�.
					void* ptr = (void*)((byte*)chunk + cmp.chunkOffset);

					return ComponentArray<ActualT>(ptr, chunk);
				}
			}


			return ComponentArray<ActualT>();
		}
	}

	inline ECSWorld::ECSWorld()
	{
		Archetype* nullArch = new Archetype();

		nullArch->full_chunks = 0;
		nullArch->componentList = adv::build_component_list(nullptr, 0);
		nullArch->componentHash = 0;
		nullArch->ownerWorld = this;

		archetypes.push_back(nullArch);

		archetypeSignatures.push_back(0);

		archetype_signature_map[0].push_back(nullArch);

		//we want archs to allways have 1 chunk at least, create initial
		adv::create_chunk_for_archetype(nullArch);
	}

	template<typename Container>
	int ECSWorld::gather_chunks(Query& query, Container& container)
	{
		int count = 0;
		adv::iterate_matching_archetypes(this, query, [&](Archetype* arch) {

			for (auto chnk : arch->chunks) {
				count++;
				container.push_back(chnk);
			}
		});
		return count;
	}

	inline void ECSWorld::destroy(EntityID eid)
	{
		adv::destroy_entity(this, eid);
	}

	template<typename Func>
	void decs::ECSWorld::for_each(Query& query, Func&& function)
	{
		using params = decltype(adv::args(&Func::operator()));

		// ���ڷ� �Ѿ�� query ���� ���� ã���� �ϴ� component type ������
		// ����ְ� �ȴ�.
		adv::iterate_matching_archetypes(this, query, [&](Archetype* arch) {

			// ���� ���ϴ� component �� ���� archetype �� ã�Ҵٸ�
			// �ش� archetype �� chunk �� ��� ��ȸ�Ѵ�.
			for (auto chnk : arch->chunks) {

				// �ش� chunk ���� ��� component �����͸� ���鼭
				// function �� �������ִ� �Լ��̴�.
				adv::unpack_chunk(params{}, chnk, function);
			}
		});
	}

	// lambda function �� ���ڷ� �޴� �Լ��̴�.
	template<typename Func>
	void decs::ECSWorld::for_each(Func&& function)
	{
		/*
		template<typename... Type>
		struct type_list {};

		template<typename Class, typename Ret, typename... Args>
		type_list<Args...> args(Ret(Class::*)(Args...) const);

		Func ��� �Լ��� ���� ��ϵ��� �����´�.
		decltype �� ���� type ���� type_list ��� struct�� type ������ �������� keyword �̴�.

		int x = 0;
		decltype(x) y = x;  // y has the same type as x (int)
		*/

		using params = decltype(adv::args(&Func::operator()));

		Query query;

		// params �� type_list<Args..> �� �ȴ�.
		// �׷��� adv::unpack_querywith ���ο�����, Args... ��� type ���� list �� ��ȸ�ϸ鼭
		// �ش� type ���� ã�� ? Query struct �� ������ְ� �Ǵ� ���̴�.
		// ��, �Ʒ� �Լ��� ���ؼ� Query.With �� ȣ���Ѵ�. �׷��� Query.require_comps ��
		// type_list<Args...> �� �ִ� ��� type ������ ���� �ȴ�.
		adv::unpack_querywith(params{}, query).build();

		for_each<Func>(query, std::move(function));
	}

	/*
	template<typename C>
	inline void ECSWorld::add_component(EntityID id, C& comp)
	{
		adv::add_component_to_entity<C>(this, id, comp);
	}
	*/

	template<typename C>
	C* ECSWorld::add_component(EntityID id)
	{
		return adv::add_component_to_entity<C>(this, id);
	}

	template<typename C>
	inline void ECSWorld::remove_component(EntityID id)
	{
		adv::remove_component_from_entity<C>(this, id);
	}


	template<typename C>
	bool ECSWorld::has_component(EntityID id)
	{
		return adv::has_component<C>(this, id);
	}

	template<typename C>
	C& ECSWorld::get_component(EntityID id)
	{
		return adv::get_entity_component<C>(this, id);
	}

	template<typename C>
	inline C* ECSWorld::set_singleton()
	{
		return set_singleton<C>(C{});
	}

	template<typename C>
	inline C* ECSWorld::set_singleton(C&& singleton)
	{
		constexpr MetatypeHash type = Metatype::build_hash<C>();

		C* old_singleton = get_singleton<C>();
		if (old_singleton) {
			*old_singleton = singleton;
			return old_singleton;
		}
		else {

			C* new_singleton = new C(singleton);
			singleton_map[type.name_hash] = (void*)new_singleton;
			return new_singleton;
		}

	}

	template<typename C>
	inline C* ECSWorld::get_singleton()
	{
		constexpr MetatypeHash type = Metatype::build_hash<C>();

		auto lookup = singleton_map.find(type.name_hash);
		if (lookup != singleton_map.end()) {
			return (C*)singleton_map[type.name_hash];
		}
		else {
			return nullptr;
		}
	}

	template<typename ...Comps>
	inline EntityID decs::ECSWorld::new_entity()
	{
		Archetype* arch = nullptr;
		//empty component list will use the hardcoded null archetype
		if constexpr (sizeof...(Comps) != 0) {
			static const Metatype* types[] = { adv::get_metatype<Comps>()... };
			constexpr size_t num = (sizeof(types) / sizeof(*types));

			adv::sort_metatypes(types, num);
			arch = adv::find_or_create_archetype(this, types, num);
		}
		else {
			arch = get_empty_archetype();
		}

		return adv::create_entity_with_archetype(arch);
	}
}
