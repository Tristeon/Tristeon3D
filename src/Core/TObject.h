#pragma once
#include <string>
#include <Core/Serializable.h>

namespace Tristeon::Core
{
	/**
	 * TObject is the base class of all Tristeon classes.
	 * Every TObject contains a name and a instanceID. The instanceID is a unique string generated upon creation or loaded in through serialization.
	 */
	class TObject : public Serializable
	{
		friend class Transform;
		friend class GameObject;
	public:
		TObject();
		virtual ~TObject() override = default;
		
		std::string name;
		[[nodiscard]] std::string getInstanceID() const;

		/**
		 * Prints the given data to the console. Only in Debug/ReleaseDebug/Editor.
		 */
		static void print(std::string data);

		nlohmann::json serialize() override;
		void deserialize(nlohmann::json json) override;
	private:
		std::string instanceID;
	};
}