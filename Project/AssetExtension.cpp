#include "AssetExtension.h"

void AssetExtension::Initialize()
{
	AssetAttribute imageAssetAttribute(AssetType::IMAGE, typeof());
	/*
	// Load the asset extension
	if (!assetExtension.LoadFromFile("Data/Extensions/AssetExtension.xml"))
	{
		cout << "Failed to load the asset extension" << endl;
		return;
	}

	// Get the root node
	TiXmlNode* root = assetExtension.FirstChild("AssetExtension");
	if (!root)
	{
		cout << "Failed to get the root node" << endl;
		return;
	}

	// Get the asset nodes
	TiXmlNode* assetNode = root->FirstChild("Asset");
	while (assetNode)
	{
		// Get the asset name
		TiXmlElement* assetElement = assetNode->ToElement();
		if (!assetElement)
		{
			cout << "Failed to get the asset element" << endl;
			return;
		}
		string assetName = assetElement->Attribute("name");

		// Get the asset type
		string assetType = assetElement->Attribute("type");

		// Get the asset path
		string assetPath = assetElement->Attribute("path");

		// Add the asset
		AddAsset(assetName, assetType, assetPath);

		// Get the next asset node
		assetNode = assetNode->NextSibling("Asset");
	}
	*/
}