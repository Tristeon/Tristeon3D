#pragma once
#include <string>
#include "Editor/Serializable.h"
#include "Editor/Draggable.h"

namespace Tristeon
{
	namespace Editor
	{
		class FolderItem;

		/**
		 FileItem is the abstract interface for editor ui that is displayed in the asset browser view
		 */
		class FileItem : public Serializable, public Draggable
		{
		public:
			
			/**
			 * \brief Create item is responsible for creating the parent relationships of the folder it is in,
			 * creating the metadata and setting file information (eg. name, extension).
			 * \param name The name of the file
			 * \param folder The folder the file is contained in
			 * \param extension The file extension of the file
			 */
			virtual void createItem(std::string name, FolderItem* folder, std::string extension = "") = 0;
			/**
			 * \brief Moves the file to a new filepath
			 */
			virtual void move(FolderItem* destination) = 0;
			/**
			 * \brief Remove the file including the meta file
			 */
			virtual void removeFile() = 0;

			bool isFolder = false;
			std::string name;
			std::string filepath;
			FolderItem* parent = nullptr;
		};
	}
}
