#include "K2Node_LowEntry_CreateJsonArray.h"

#include "LowEntryJsonLibrary.h"

#include "LowEntryJsonValue.h"
#include "LowEntryJsonArray.h"


#include "EdGraph/EdGraphPin.h"
#include "Engine/Blueprint.h"
#include "Framework/Commands/UIAction.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "EdGraphSchema_K2.h"
#include "EdGraph/EdGraphNodeUtils.h"
#include "Kismet2/BlueprintEditorUtils.h"

#include "Kismet/KismetArrayLibrary.h"
#include "ScopedTransaction.h"
#include "EdGraphUtilities.h"
#include "KismetCompiledFunctionContext.h"
#include "KismetCompilerMisc.h"
#include "BlueprintNodeSpawner.h"
#include "EditorCategoryUtils.h"
#include "BlueprintActionDatabaseRegistrar.h"


#define LOCTEXT_NAMESPACE "MakeArrayNode"


/////////////////////////////////////////////////////
// FKCHandler_LowEntry_CreateJsonArray

class FKCHandler_LowEntry_CreateJsonArray : public FNodeHandlingFunctor
{
public:
	FKCHandler_LowEntry_CreateJsonArray(FKismetCompilerContext& InCompilerContext) : FNodeHandlingFunctor(InCompilerContext)
	{
	}

	virtual void RegisterNets(FKismetFunctionContext& Context, UEdGraphNode* Node) override
	{
		UK2Node_LowEntry_CreateJsonArray* ArrayNode = CastChecked<UK2Node_LowEntry_CreateJsonArray>(Node);
		FNodeHandlingFunctor::RegisterNets(Context, Node);

		// Create a local term to drop the array into
		UEdGraphPin* ArrayOutputPin = ArrayNode->GetArrayOutputPin();
		FBPTerminal* ArrayTerm = Context.CreateLocalTerminalFromPinAutoChooseScope(ArrayOutputPin, Context.NetNameMap->MakeValidName(ArrayOutputPin) + TEXT("_Array"));
		ArrayTerm->bPassedByReference = false;
		ArrayTerm->Source = Node;
		Context.NetMap.Add(ArrayOutputPin, ArrayTerm);

		// Create a local term to drop the object into
		UEdGraphPin* ObjectOutputPin = ArrayNode->GetObjectOutputPin();
		FBPTerminal* ObjectTerm = Context.CreateLocalTerminalFromPinAutoChooseScope(ObjectOutputPin, Context.NetNameMap->MakeValidName(ObjectOutputPin) + TEXT("_Object"));
		ObjectTerm->bPassedByReference = false;
		ObjectTerm->Source = Node;
		Context.NetMap.Add(ObjectOutputPin, ObjectTerm);
	}

	virtual void Compile(FKismetFunctionContext& Context, UEdGraphNode* Node) override
	{
		UK2Node_LowEntry_CreateJsonArray* ArrayNode = CastChecked<UK2Node_LowEntry_CreateJsonArray>(Node);
		UEdGraphPin* ArrayOutputPin = ArrayNode->GetArrayOutputPin();
		UEdGraphPin* ObjectOutputPin = ArrayNode->GetObjectOutputPin();

		FBPTerminal** ArrayOutputVariable = Context.NetMap.Find(ArrayOutputPin);
		FBPTerminal** ObjectOutputVariable = Context.NetMap.Find(ObjectOutputPin);
		check(ArrayOutputVariable);
		check(ObjectOutputVariable);

		
		FBlueprintCompiledStatement& CreateArrayStatement = Context.AppendStatementForNode(Node);
		CreateArrayStatement.Type = KCST_CreateArray;
		CreateArrayStatement.LHS = *ArrayOutputVariable;

		for(auto PinIt = Node->Pins.CreateIterator(); PinIt; ++PinIt)
		{
			UEdGraphPin* Pin = *PinIt;
			if(Pin && (Pin->Direction == EGPD_Input) && (Pin->PinType.PinCategory != UEdGraphSchema_K2::PC_Exec))
			{
				FBPTerminal** InputTerm = Context.NetMap.Find(FEdGraphUtilities::GetNetFromPin(Pin));
				if(InputTerm)
				{
					CreateArrayStatement.RHS.Add(*InputTerm);
				}
			}
		}


		UClass* ConvertFunctionClass = Cast<UClass>(ULowEntryJsonLibrary::StaticClass());
		UFunction* ConvertFunctionPointer = FindField<UFunction>(ConvertFunctionClass, TEXT("Array_Pure_CreateFromJsonValueArray"));
		check(ConvertFunctionPointer);

		FBlueprintCompiledStatement& CreateConvertStatement = Context.AppendStatementForNode(Node);
		CreateConvertStatement.Type = KCST_CallFunction;
		CreateConvertStatement.FunctionToCall = ConvertFunctionPointer;
		CreateConvertStatement.LHS = *ObjectOutputVariable;
		CreateConvertStatement.RHS.Add(*ArrayOutputVariable);


		if(!false)
		{
			GenerateSimpleThenGoto(Context, *Node);
		}
	}
};


/////////////////////////////////////////////////////
// UK2Node_LowEntry_CreateJsonArray

UK2Node_LowEntry_CreateJsonArray::UK2Node_LowEntry_CreateJsonArray(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	NumInputs = 1;
}

FNodeHandlingFunctor* UK2Node_LowEntry_CreateJsonArray::CreateNodeHandler(FKismetCompilerContext& CompilerContext) const
{
	return new FKCHandler_LowEntry_CreateJsonArray(CompilerContext);
}

FText UK2Node_LowEntry_CreateJsonArray::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return FText::FromString("Create Json Array");
}

UEdGraphPin* UK2Node_LowEntry_CreateJsonArray::GetArrayOutputPin() const
{
	return FindPin(TEXT("TempArray"));
}

UEdGraphPin* UK2Node_LowEntry_CreateJsonArray::GetObjectOutputPin() const
{
	return FindPin(TEXT("JsonArray"));
}

bool UK2Node_LowEntry_CreateJsonArray::IsNodePure() const
{
	return false;
}

void UK2Node_LowEntry_CreateJsonArray::AllocateDefaultPins()
{
	if(!false)
	{
		CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Exec, TEXT(""), NULL, false, false, UEdGraphSchema_K2::PN_Execute);
		CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, TEXT(""), NULL, false, false, UEdGraphSchema_K2::PN_Then);
	}

	// Create the output pins
	UEdGraphPin* ObjectOutputPin = CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Object, TEXT(""), ULowEntryJsonArray::StaticClass(), false, false, TEXT("JsonArray"));
	UEdGraphPin* ArrayOutputPin = CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Object, TEXT(""), ULowEntryJsonValue::StaticClass(), true, false, TEXT("TempArray"));

	ArrayOutputPin->bHidden = true;

	// Create the input pins to create the arrays from
	for(int32 i = 0; i < NumInputs; ++i)
	{
		CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Object, TEXT(""), ULowEntryJsonValue::StaticClass(), false, false, *FString::Printf(TEXT("[%d]"), i));
	}
}

void UK2Node_LowEntry_CreateJsonArray::NotifyPinConnectionListChanged(UEdGraphPin* Pin)
{
	Super::NotifyPinConnectionListChanged(Pin);

	if(Pin != nullptr)
	{
		if((Pin->Direction != EGPD_Input) || (Pin->PinType.PinCategory == UEdGraphSchema_K2::PC_Exec))
		{
			return;
		}
	}

	bool DisconnectedPinEncountered = false;
	int32 FirstDisconnectedPinSinceLastConnected = -1;
	for(int32 PinIndex = 0; PinIndex < Pins.Num(); ++PinIndex)
	{
		if((Pins[PinIndex]->Direction == EGPD_Input) && (Pins[PinIndex]->PinType.PinCategory != UEdGraphSchema_K2::PC_Exec))
		{
			if(Pins[PinIndex]->LinkedTo.Num() <= 0)
			{
				if(FirstDisconnectedPinSinceLastConnected == -1)
				{
					FirstDisconnectedPinSinceLastConnected = PinIndex;
				}
				if(!DisconnectedPinEncountered)
				{
					DisconnectedPinEncountered = true;
				}
			}
			else
			{
				FirstDisconnectedPinSinceLastConnected = -1;
			}
		}
	}

	if(!DisconnectedPinEncountered)
	{
		FScopedTransaction Transaction(LOCTEXT("AddPinTx", "AddPin"));
		Modify();

		++NumInputs;
		FEdGraphPinType OutputPinType = GetArrayOutputPin()->PinType;
		CreatePin(EGPD_Input, OutputPinType.PinCategory, OutputPinType.PinSubCategory, OutputPinType.PinSubCategoryObject.Get(), false, false, *FString::Printf(TEXT("[%d]"), (NumInputs - 1)));
		const bool bIsCompiling = GetBlueprint()->bBeingCompiled;
		if(!bIsCompiling)
		{
			FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(GetBlueprint());
		}
		return;
	}

	if((FirstDisconnectedPinSinceLastConnected != -1) && (Pins.Num() - 1 > FirstDisconnectedPinSinceLastConnected))
	{
		FScopedTransaction Transaction(LOCTEXT("RemovePinTx", "RemovePin"));
		Modify();

		for(int32 PinIndex = Pins.Num() - 1; PinIndex > FirstDisconnectedPinSinceLastConnected; --PinIndex)
		{
			UEdGraphPin* RemovePin = Pins[PinIndex];
			RemovePin->Modify();
			RemovePin->BreakAllPinLinks();
			Pins.RemoveAt(PinIndex);
			--NumInputs;
		}
		const bool bIsCompiling = GetBlueprint()->bBeingCompiled;
		if(!bIsCompiling)
		{
			FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(GetBlueprint());
		}
	}
}

FText UK2Node_LowEntry_CreateJsonArray::GetTooltipText() const
{
	return FText::FromString("Creates a new Json Array.");
}

void UK2Node_LowEntry_CreateJsonArray::AddInputPin()
{
	FScopedTransaction Transaction(LOCTEXT("AddPinTx", "AddPin"));
	Modify();

	++NumInputs;
	FEdGraphPinType OutputPinType = GetArrayOutputPin()->PinType;
	CreatePin(EGPD_Input, OutputPinType.PinCategory, OutputPinType.PinSubCategory, OutputPinType.PinSubCategoryObject.Get(), false, false, *FString::Printf(TEXT("[%d]"), (NumInputs - 1)));

	const bool bIsCompiling = GetBlueprint()->bBeingCompiled;
	if(!bIsCompiling)
	{
		FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(GetBlueprint());
	}
}

void UK2Node_LowEntry_CreateJsonArray::RemoveInputPin(UEdGraphPin* Pin)
{
	FScopedTransaction Transaction(LOCTEXT("RemovePinTx", "RemovePin"));
	Modify();

	check(((Pin->Direction == EGPD_Input) && (Pin->PinType.PinCategory != UEdGraphSchema_K2::PC_Exec)));

	int32 PinRemovalIndex = INDEX_NONE;
	if(Pins.Find(Pin, /*out*/ PinRemovalIndex))
	{
		for(int32 PinIndex = PinRemovalIndex + 1; PinIndex < Pins.Num(); ++PinIndex)
		{
			Pins[PinIndex]->Modify();
			Pins[PinIndex]->PinName = FString::Printf(TEXT("[%d]"), PinIndex - 2); // -1 to shift back one, -1 to account for the output pin at the 0th position
		}

		Pin->Modify();
		Pin->BreakAllPinLinks();
		Pins.RemoveAt(PinRemovalIndex);

		--NumInputs;

		FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(GetBlueprint());
	}
}

void UK2Node_LowEntry_CreateJsonArray::GetContextMenuActions(const FGraphNodeContextMenuBuilder& Context) const
{
	Super::GetContextMenuActions(Context);

	if(!Context.bIsDebugging)
	{
		Context.MenuBuilder->BeginSection("K2NodeMakeArray", FText::FromString("Low Entry"));

		if(Context.Pin != NULL)
		{
			// we only do this for normal BlendList/BlendList by enum, BlendList by Bool doesn't support add/remove pins
			if((Context.Pin->Direction == EGPD_Input) && (Context.Pin->PinType.PinCategory != UEdGraphSchema_K2::PC_Exec))
			{
				//@TODO: Only offer this option on arrayed pins
				Context.MenuBuilder->AddMenuEntry(
					LOCTEXT("RemovePin", "Remove array element pin"),
					LOCTEXT("RemovePinTooltip", "Remove this array element pin"),
					FSlateIcon(),
					FUIAction(
					FExecuteAction::CreateUObject(this, &UK2Node_LowEntry_CreateJsonArray::RemoveInputPin, const_cast<UEdGraphPin*>(Context.Pin))
					)
					);
			}
		}
		else
		{
			Context.MenuBuilder->AddMenuEntry(
				LOCTEXT("AddPin", "Add array element pin"),
				LOCTEXT("AddPinTooltip", "Add another array element pin"),
				FSlateIcon(),
				FUIAction(
				FExecuteAction::CreateUObject(this, &UK2Node_LowEntry_CreateJsonArray::AddInputPin)
				)
				);
		}

		Context.MenuBuilder->EndSection();
	}
}

void UK2Node_LowEntry_CreateJsonArray::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	// actions get registered under specific object-keys; the idea is that 
	// actions might have to be updated (or deleted) if their object-key is  
	// mutated (or removed)... here we use the node's class (so if the node 
	// type disappears, then the action should go with it)
	UClass* ActionKey = GetClass();
	// to keep from needlessly instantiating a UBlueprintNodeSpawner, first   
	// check to make sure that the registrar is looking for actions of this type
	// (could be regenerating actions for a specific asset, and therefore the 
	// registrar would only accept actions corresponding to that asset)
	if(ActionRegistrar.IsOpenForRegistration(ActionKey))
	{
		UBlueprintNodeSpawner* NodeSpawner = UBlueprintNodeSpawner::Create(GetClass());
		check(NodeSpawner != nullptr);

		ActionRegistrar.AddBlueprintAction(ActionKey, NodeSpawner);
	}
}

FText UK2Node_LowEntry_CreateJsonArray::GetMenuCategory() const
{
	return FText::FromString("Low Entry|Json|Array|Create (Callable)");
}

#undef LOCTEXT_NAMESPACE
