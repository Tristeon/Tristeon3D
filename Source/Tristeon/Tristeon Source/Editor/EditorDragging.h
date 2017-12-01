#pragma once

namespace Tristeon
{
	namespace Editor
	{
		class Draggable;

		class EditorDragging
		{
		public:
			/**
			* \return Returns the current draggingItem. Any draggable object can be selected
			*/
			static Draggable* getDragableItem() { return draggingItem; }
			/**
			* \param draggingItem The item to be set as the dragable item
			*/
			static void setDragableItem(Draggable* draggingItem) { EditorDragging::draggingItem = draggingItem; }
			/**
			 * \brief sets the currently dragging item to equal nullptr
			 */
			static void reset() { draggingItem = nullptr; }
		private:
			static Draggable* draggingItem;
		};
	}
}
