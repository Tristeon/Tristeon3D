#pragma once
#include "FileItem.h"
#include <vector>
#include "Editor/TypeRegister.h"

namespace Tristeon
{
	namespace Editor
	{
		class FileItemManager;

		/**
		 * \brief Folder represented in the editor, responsible for containing fileitems,
		 * loading their metadata and displaying their contents
		 */
		class FolderItem : public FileItem
		{
		public:
			FolderItem();
			~FolderItem();

			/**
			* \brief Create item is responsible for creating the parent relationships of the folder,
			* creating the metadata and directory of the folder and setting its file information (eg. name, extension).
			* \param name The name of the folder
			* \param folder The folder the folder is contained in
			* \param extension There is no implementation of extension in the folderitem class
			*/
			void init(std::string name, FolderItem* folder, std::string extension = "") override;

			bool setupDone = false;

			/**
			 * \brief Create only the metadata
			 */
			void createMetaData(const std::string& filepath);
			/**
			 * \brief setup checks all of the files that the folder contains and will verify
			 * if there are missing meta files or if the some of the files have been deleted
			 * \param doChildren Iterate setup over children folders too
			 */
			void setup(bool doChildren = false);

			/**
			 * \brief The files the folder contains
			 */
			std::vector<FileItem*> fileItems;
			/**
			 * \brief The folders the folder contains
			 */
			std::vector<FolderItem*> children;

			/**
			 * \brief Checks if the destination is a child of this folder
			 */
			bool hasChild(FolderItem* destination);
			/**
			 * \brief Moves the folder to a new filepath
			 */
			void move(FolderItem* destination) override;
			/**
			 * \brief Draws a node tree of the parent-child structure of the folder
			 */
			void drawHierarchy(FileItemManager* itemManager);

			nlohmann::json serialize() override;
			void deserialize(nlohmann::json json) override;

			/**
			 * \brief Removes the original file and deletes the metadata
			 */
			void removeFile() override;
		private:
			static DerivedRegister<FolderItem> reg;
			int selectionMask;
		};
	}
}
