#pragma once
class EditorContext
{
public :
	// ������ ��� ������ ����.
	struct Directories
	{
		/**�ַ�� ���� ���ڿ� ("Solution")
		 */
		static const char* solution;

		/**���� ���� ���ڿ� ("Engine")
		 */
		static const char* engine;

		/**�ַ�� ���� ���ڿ� ("Project")
		 */
		static const char* project;

		/**���� ���� ���ڿ� ("Settings")
		 */
		static const char* settings;

		/**�ַ�� ���� ���ڿ� ("Resources")
		 */
		static const char* resources;

		/**���� ���� ���ڿ� ("Engine")
		 */
		static const char* bundle;

		/**builtin/Resource ����
		 */
		static const char* builtinresource;

		/**�ҽ� ���� ���ڿ� ("Program")
		 */
		static const char* program;

		/**library ����
		 */
		static const char* library;

		/**builtinShader ����
		 */
		static const char* builtinShader;

		/**Log ����
		 */
		static const char* log;

		/**builtin ����
		 */
		static const char* builtin;
	};

};

