#pragma once

#pragma once

#include <nlohmann/json.hpp>
using json = nlohmann::json;

namespace Serialization
{
	enum : std::uint32_t
	{
		kSerializationVersion = 1,
		kUniqueID = '@CNO'
	};
	
	//void SaveCallback(SKSE::SerializationInterface* a_intfc);
	void LoadCallback(SKSE::SerializationInterface* a_intfc);
	void RevertCallback(SKSE::SerializationInterface*);
}
