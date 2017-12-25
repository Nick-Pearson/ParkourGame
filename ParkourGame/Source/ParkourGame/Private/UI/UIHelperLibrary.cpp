#include "UIHelperLibrary.h"
#include "HttpModule.h"
#include "IHttpResponse.h"

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

void UUIHelperLibrary::GetServerList(FDNSResultReturnEvent event){
    TSharedRef<IHttpRequest> HttpReq = FHttpModule::Get().CreateRequest();
    
    HttpReq->SetVerb("GET");
    HttpReq->SetURL("https://api.parkour.ultra-horizon.com/servers");
    
    HttpReq->OnProcessRequestComplete().BindStatic(&UUIHelperLibrary::ProcessResponse, event);
    
    HttpReq->ProcessRequest();
}

void UUIHelperLibrary::ProcessResponse(FHttpRequestPtr ReqPtr, FHttpResponsePtr Response, bool Success, FDNSResultReturnEvent event){
    if (!Success){
        return;
    }
    TArray<FString> servers = {Response->GetContentAsString()};
    event.Execute(servers);
}
