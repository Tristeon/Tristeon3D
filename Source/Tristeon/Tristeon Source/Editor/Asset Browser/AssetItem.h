#pragma once
#include "FileItem.h"
#include "Editor/TypeRegister.h"


namespace Tristeon
{
	namespace Editor
	{
		/**
		 * \brief An assetitem is any file that can be shown by the editor which is not a folder
		 */
		class AssetItem : public FileItem
		{
		public:
			AssetItem();
			~AssetItem();

			nlohmann::json serialize() override;
			void deserialize(nlohmann::json json) override;

			/**
			* \brief Create item is responsible for creating the parent relationships of the folder it is in,
			* creating the metadata and setting file information (eg. name, extension).
			* \param name The name of the file
			* \param folder The folder the file is contained in
			* \param extension The file extension of the file
			*/
			void createItem(std::string name, FolderItem* folder, std::string extension = "") override;
			void move(FolderItem* destination) override;

			void removeFile() override;
			virtual void onDoubleClick();

			std::string extension;
		private:
			static DerivedRegister<AssetItem> reg;
		};
	}
}
