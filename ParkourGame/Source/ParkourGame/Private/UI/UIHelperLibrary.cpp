#include "UIHelperLibrary.h"

FText UUIHelperLibrary::FormatToasterMessage(const FText& FormatString, const TArray<FText>& Parameters)
{
	FFormatOrderedArguments args;
	args.Reserve(Parameters.Num());

	for (const FText& param : Parameters)
	{
		args.Add(FFormatArgumentValue(param));
	}

	return FText::Format(FTextFormat(FormatString), args);
}
