#include <src/models/ProjectTreeHkHandler.h>
#include <src/hkx/HkxLinkedTableVariant.h>

#include <hkbCharacterData_7.h>
#include <hkbBehaviorGraph_1.h>
#include <hkbBehaviorGraphData_2.h>
#include <hkbBehaviorGraphStringData_1.h>
#include <hkbStateMachine_4.h>
#include <hkbStateMachineStateInfo_4.h>
#include <hkbStateMachineTransitionInfoArray_0.h>

using namespace ckcmd::HKX;


struct  HandleCharacterData
{
	static const size_t DATA_SUPPORTS = 3;

	static const char* DataListsName(int row)
	{
		switch ((NodeType)row) {
		case NodeType::deformableSkinNames:
			return "Deformable Skins";
		case NodeType::animationNames:
			return "Animations";
		case NodeType::characterPropertyNames:
			return "Character Properties";
		default:
			break;
		}
		return "Invalid Character Entry";
	};

	static int getChildCount(hkVariant* variant, NodeType childType)
	{
		auto* data = reinterpret_cast<hkbCharacterData*>(variant->m_object);
		if (data == NULL)
			return 0;
		auto string_data = data->m_stringData;
		if (string_data == NULL)
			return 0;
		if (childType == NodeType::ProjectNode)
		{
			return DATA_SUPPORTS;
		}
		else if (childType == NodeType::deformableSkinNames)
		{
			return string_data->m_deformableSkinNames.getSize();
		}
		else if (childType == NodeType::animationNames)
		{
			return string_data->m_animationNames.getSize();
		}
		else if (childType == NodeType::characterPropertyNames)
		{
			return string_data->m_characterPropertyNames.getSize();
		}
		else {
			return 0;
		}
	}

	static QVariant data(int row, int column, hkVariant* variant, NodeType childType)
	{
		auto* data = reinterpret_cast<hkbCharacterData*>(variant->m_object);
		auto string_data = data->m_stringData;
		if (childType == NodeType::deformableSkinNames ||
			childType == NodeType::animationNames ||
			childType == NodeType::characterPropertyNames)
		{
			return DataListsName((int)childType);
		}
		else if (childType == NodeType::deformableSkinName)
		{
			return string_data->m_deformableSkinNames[row].cString();
		}
		else if (childType == NodeType::animationName)
		{
			return string_data->m_animationNames[row].cString();
		}
		else if (childType == NodeType::characterPropertyName)
		{
			return string_data->m_characterPropertyNames[row].cString();
		}
		else {
			return 0;
		}
	}

	static ModelEdge get_child(int index, int file, hkVariant* variant, ResourceManager& manager, NodeType childType)
	{
		auto* data = reinterpret_cast<hkbCharacterData*>(variant->m_object);
		auto string_data = data->m_stringData;
		if (childType == NodeType::ProjectNode)
		{
			switch (index) {
			case 0:
				return ModelEdge(variant, file, index, 0, variant, NodeType::deformableSkinNames);
			case 1:
				return ModelEdge(variant, file, index, 0, variant, NodeType::animationNames);
			case 2:
				return ModelEdge(variant, file, index, 0, variant, NodeType::characterPropertyNames);
			default:
				break;
			}
			return ModelEdge();
		}
		else if (childType == NodeType::deformableSkinNames)
		{
			return ModelEdge(variant, file, index, 0, variant, NodeType::deformableSkinName);
		}
		else if (childType == NodeType::animationNames)
		{
			return ModelEdge(variant, file, index, 0, variant, NodeType::animationName);
		}
		else if (childType == NodeType::characterPropertyNames)
		{
			return ModelEdge(variant, file, index, 0, variant, NodeType::characterPropertyName);
		}
		else {
			return ModelEdge();
		}
	}
};

struct  HandleBehaviorData
{

	static const size_t DATA_SUPPORTS = 3;

	static const char* DataListsName(int row)
	{
		switch ((NodeType)row) {
		case NodeType::behaviorEventNames:
			return "Events";
		case NodeType::behaviorVariableNames:
			return "Variables";
		case NodeType::behaviorCharacterPropertyNames:
			return "Properties";
		default:
			break;
		}
		return "Invalid Behavior Entry";
	};

	static int getChildCount(hkVariant* variant, NodeType childType)
	{
		auto* graph = reinterpret_cast<hkbBehaviorGraph*>(variant->m_object);
		if (graph == NULL)
			return 0;
		auto data = graph->m_data;
		if (data == NULL)
			return 0;
		auto string_data = data->m_stringData;
		if (string_data == NULL)
			return 0;
		if (childType == NodeType::ProjectNode)
		{
			return DATA_SUPPORTS + HkxLinkedTableVariant(*variant).links().size();
		}
		else if (childType == NodeType::behaviorEventNames)
		{
			return string_data->m_eventNames.getSize();
		}
		else if (childType == NodeType::behaviorVariableNames)
		{
			return string_data->m_variableNames.getSize();
		}
		else if (childType == NodeType::behaviorCharacterPropertyNames)
		{
			return string_data->m_characterPropertyNames.getSize();
		}
		else if (childType == NodeType::behaviorEventName)
		{
			return 0;
		}
		else if (childType == NodeType::behaviorVariableName)
		{
			return 0;
		}
		else if (childType == NodeType::behaviorCharacterPropertyName)
		{
			return 0;
		}
		return HkxLinkedTableVariant(*variant).links().size();
	}

	static QVariant data(int row, int column, hkVariant* variant, NodeType childType)
	{
		auto* data = reinterpret_cast<hkbBehaviorGraph*>(variant->m_object);
		auto string_data = data->m_data->m_stringData;
		if (childType == NodeType::behaviorEventNames ||
			childType == NodeType::behaviorVariableNames ||
			childType == NodeType::behaviorCharacterPropertyNames)
		{
			return DataListsName((int)childType);
		}
		else if (childType == NodeType::behaviorEventName)
		{
			return string_data->m_eventNames[row].cString();
		}
		else if (childType == NodeType::behaviorVariableName)
		{
			return string_data->m_variableNames[row].cString();
		}
		else if (childType == NodeType::behaviorCharacterPropertyName)
		{
			return string_data->m_characterPropertyNames[row].cString();
		}
		else {
			if (column == 0)
			{
				return HkxVariant(*variant).name();
			}
			HkxLinkedTableVariant v(*variant);
			return v.data(row - DATA_SUPPORTS, column - 1);
		}
	}

	static ModelEdge get_child(int index, int file, hkVariant* variant, ResourceManager& manager, NodeType childType)
	{
		auto* data = reinterpret_cast<hkbCharacterData*>(variant->m_object);
		auto string_data = data->m_stringData;
		if (childType == NodeType::ProjectNode)
		{
			switch (index) {
			case 0:
				return ModelEdge(variant, file, index, 0, variant, NodeType::behaviorEventNames);
			case 1:
				return ModelEdge(variant, file, index, 0, variant, NodeType::behaviorVariableNames);
			case 2:
				return ModelEdge(variant, file, index, 0, variant, NodeType::behaviorCharacterPropertyNames);
			default:
				break;
			}
		}
		else if (childType == NodeType::behaviorEventNames)
		{
			return ModelEdge(variant, file, index, 0, variant, NodeType::behaviorEventName);
		}
		else if (childType == NodeType::behaviorVariableNames)
		{
			return ModelEdge(variant, file, index, 0, variant, NodeType::behaviorVariableName);
		}
		else if (childType == NodeType::behaviorCharacterPropertyNames)
		{
			return ModelEdge(variant, file, index, 0, variant, NodeType::behaviorCharacterPropertyName);
		}
		HkxLinkedTableVariant v(*variant);
		auto& links = v.links();
		auto& link = links.at(index - DATA_SUPPORTS);
		int file_index = manager.findIndex(file, link._ref);
		if (file_index == -1)
			__debugbreak();
		return ModelEdge((ProjectNode*)nullptr, file, link._row, link._column, manager.at(file, file_index));
	}
};

struct  HandleStateMachineData
{

	static const size_t DATA_SUPPORTS = 2;

	static const char* DataListsName(int row)
	{
		switch ((NodeType)row) {
		case NodeType::FSMWildcardTransitions:
			return "Wildcard Transitions";
		case NodeType::FSMStateTransitions:
			return "Transitions";
		default:
			break;
		}
		return "Invalid FSM Entry";
	};

	static int getChildCount(hkVariant* variant, NodeType childType)
	{
		auto* FSM = reinterpret_cast<hkbStateMachine*>(variant->m_object);
		if (FSM == NULL)
			return 0;

		if (childType == NodeType::ProjectNode || childType == NodeType::HavokNative)
		{
			return DATA_SUPPORTS + HkxLinkedTableVariant(*variant).links().size();
		}
		else if (childType == NodeType::FSMWildcardTransitions)
		{
			if (FSM->m_wildcardTransitions == NULL)
				return 0;
			return FSM->m_wildcardTransitions->m_transitions.getSize();
		}
		else if (childType == NodeType::FSMWildcardTransition)
		{
			return 0;
		}
		else if (childType == NodeType::FSMStateTransitions)
		{
			int count = 0;
			for (int i = 0; i < FSM->m_states.getSize(); i++)
			{
				if (NULL != FSM->m_states[i]->m_transitions)
					count += FSM->m_states[i]->m_transitions->m_transitions.getSize();
			}
			return count;
		}
		else if (childType == NodeType::FSMStateTransition)
		{
			return 0;
		}
		
		return HkxLinkedTableVariant(*variant).links().size();
	}

	static QVariant data(int row, int column, hkVariant* variant, NodeType childType)
	{
		auto* FSM = reinterpret_cast<hkbStateMachine*>(variant->m_object);
		if (FSM == NULL)
			return 0;

		if (childType == NodeType::FSMWildcardTransitions || childType == NodeType::FSMStateTransitions)
		{
			return DataListsName((int)childType);
		}
		else if (childType == NodeType::FSMWildcardTransition)
		{
			auto& transition = FSM->m_wildcardTransitions->m_transitions[row];
			QString result = "Transition to Invalid State";
			for (const auto& state : FSM->m_states)
			{
				if (state->m_stateId == transition.m_toStateId)
				{
					result = QString("Transition to ") + state->m_name.cString();
				}
			}
			return result;
		}
		else if (childType == NodeType::FSMStateTransition)
		{
			int index = row;
			QString result = "Invalid state transition";
			for (const auto& state : FSM->m_states)
			{
				if (NULL != state->m_transitions)
				{
					if (index < state->m_transitions->m_transitions.getSize())
					{
						auto& transition = state->m_transitions->m_transitions[index];
						for (const auto& to_state : FSM->m_states)
						{
							if (to_state->m_stateId == transition.m_toStateId)
							{
								result = QString("Transition from %1 to %2 ").arg(state->m_name.cString()).arg(to_state->m_name.cString());
								break;
							}
						}				
						break;
					}
					index -= state->m_transitions->m_transitions.getSize();
				}
			}
			return result;
		}
		if (column == 0)
		{
			return HkxVariant(*variant).name();
		}
		HkxLinkedTableVariant v(*variant);
		if (FSM->m_wildcardTransitions != NULL)
			return v.data(row-DATA_SUPPORTS, column - 1);
		return v.data(row, column - 1);
	}

	static ModelEdge get_child(int index, int file, hkVariant* variant, ResourceManager& manager, NodeType childType)
	{
		auto* data = reinterpret_cast<hkbCharacterData*>(variant->m_object);
		auto string_data = data->m_stringData;
		if (childType == NodeType::ProjectNode || childType == NodeType::HavokNative)
		{
			switch (index) {
			case 0:
				return ModelEdge(variant, file, index, 0, variant, NodeType::FSMWildcardTransitions);
			case 1:
				return ModelEdge(variant, file, index, 0, variant, NodeType::FSMStateTransitions);
			default:
				break;
			}
		}
		else if (childType == NodeType::FSMWildcardTransitions)
		{
			return ModelEdge(variant, file, index, 0, variant, NodeType::FSMWildcardTransition);
		}
		else if (childType == NodeType::FSMStateTransitions)
		{
			return ModelEdge(variant, file, index, 0, variant, NodeType::FSMStateTransition);
		}
		HkxLinkedTableVariant v(*variant);
		auto& links = v.links();
		auto& link = links.at(index - DATA_SUPPORTS);
		int file_index = manager.findIndex(file, link._ref);
		if (file_index == -1)
			__debugbreak();
		return ModelEdge((ProjectNode*)nullptr, file, link._row, link._column, manager.at(file, file_index));
	}
};


int ProjectTreeHkHandler::getChildCount(hkVariant* variant, NodeType childType)
{
	if (&hkbCharacterDataClass == variant->m_class)
	{
		return HandleCharacterData::getChildCount(variant, childType);
	}
	if (&hkbBehaviorGraphClass == variant->m_class)
	{
		return HandleBehaviorData::getChildCount(variant, childType);
	}
	if (&hkbStateMachineClass == variant->m_class)
	{
		return HandleStateMachineData::getChildCount(variant, childType);
	}
	return HkxLinkedTableVariant(*variant).links().size();
}

QVariant ProjectTreeHkHandler::data(int row, int column, hkVariant* variant, NodeType childType)
{
	if (&hkbCharacterDataClass == variant->m_class)
	{
		return HandleCharacterData::data(row, column, variant, childType);
	}
	if (&hkbBehaviorGraphClass == variant->m_class)
	{
		return HandleBehaviorData::data(row, column, variant, childType);
	}
	if (&hkbStateMachineClass == variant->m_class)
	{
		return HandleStateMachineData::data(row, column, variant, childType);
	}
	if (column == 0)
	{
		return HkxVariant(*variant).name();
	}
	HkxLinkedTableVariant v(*variant);
	return v.data(row, column - 1);
}

ModelEdge ProjectTreeHkHandler::get_child(int index, int file, hkVariant* variant, ResourceManager& manager, NodeType childType)
{
	if (&hkbCharacterDataClass == variant->m_class)
	{
		return HandleCharacterData::get_child(index, file, variant, manager, childType);
	}
	if (&hkbBehaviorGraphClass == variant->m_class)
	{
		return HandleBehaviorData::get_child(index, file, variant, manager, childType);
	}
	if (&hkbStateMachineClass == variant->m_class)
	{
		return HandleStateMachineData::get_child(index, file, variant, manager, childType);
	}
	HkxLinkedTableVariant v(*variant);
	auto& links = v.links();
	auto& link = links.at(index);
	int file_index = manager.findIndex(file, link._ref);
	if (file_index == -1)
		__debugbreak();
	return ModelEdge((ProjectNode*)nullptr, file, link._row, link._column, manager.at(file, file_index));
}