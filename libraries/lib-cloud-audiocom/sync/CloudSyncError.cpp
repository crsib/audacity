/*  SPDX-License-Identifier: GPL-2.0-or-later */
/*!********************************************************************

  Audacity: A Digital Audio Editor

  CloudSyncError.cpp

  Dmitry Vedenko

**********************************************************************/
#include "CloudSyncError.h"

#include <unordered_map>

#include "IResponse.h"

#include "CodeConversions.h"
#include "TranslatableString.h"

namespace audacity::cloud::audiocom::sync
{
CLOUD_AUDIOCOM_API CloudSyncError
DeduceUploadError(audacity::network_manager::IResponse& response)
{
   using namespace audacity::network_manager;

   const auto error = response.getError();

   if (error == NetworkError::NoError)
      return {};

   CloudSyncError result;

   if (error != NetworkError::HTTPError)
   {
      result.Type = error == NetworkError::OperationCancelled ?
                       CloudSyncError::Cancelled :
                       CloudSyncError::Network;

      result.ErrorMessage = response.getErrorString();

      return result;
   }

   const auto statusCode = response.getHTTPCode();

   static const std::unordered_map<int, CloudSyncError::ErrorType> errors {
      { HttpCode::Unauthorized, CloudSyncError::Authorization },
      { HttpCode::PaymentRequired, CloudSyncError::ProjectLimitReached },
      { HttpCode::Forbidden, CloudSyncError::Authorization },
      { HttpCode::NotFound, CloudSyncError::ProjectNotFound },
      { HttpCode::RequestTimeout, CloudSyncError::Network },
      { HttpCode::Conflict, CloudSyncError::ProjectVersionConflict },
      { HttpCode::Gone, CloudSyncError::ProjectNotFound },
      { HttpCode::PayloadTooLarge, CloudSyncError::ProjectStorageLimitReached },
      { HttpCode::UnprocessableEntity, CloudSyncError::ProjectVersionConflict },
      { HttpCode::GatewayTimeout, CloudSyncError::Network },
      { HttpCode::HTTPVersionNotSupported, CloudSyncError::Network },
      { HttpCode::NetworkAuthenticationRequired, CloudSyncError::Network },
   };

   const auto it = errors.find(statusCode);

   result.Type = it == errors.end() ? CloudSyncError::Server : it->second;

   result.ErrorMessage = std::string("HTTP ") + std::to_string(statusCode) +
                         std::string("\n") + response.readAll<std::string>();

   return result;
}

CloudSyncError MakeClientFailure(const TranslatableString& message)
{
   return { CloudSyncError::ClientFailure,
            audacity::ToUTF8(message.Translation()) };
}

CloudSyncError MakeClientFailure(const std::string& message)
{
   return { CloudSyncError::ClientFailure, message };
}

CloudSyncError MakeClientFailure(const char* message)
{
   return { CloudSyncError::ClientFailure, message };
}

CloudSyncError::ErrorType DeduceError(ResponseResultCode code)
{
   switch (code)
   {
   case ResponseResultCode::Success:
      return CloudSyncError::None;
   case ResponseResultCode::Cancelled:
      return CloudSyncError::Cancelled;
   case ResponseResultCode::Expired:
      return CloudSyncError::DataUploadFailed;
   case ResponseResultCode::Conflict:
      return CloudSyncError::ProjectVersionConflict;
   case ResponseResultCode::ConnectionFailed:
      return CloudSyncError::Network;
   case ResponseResultCode::PaymentRequired:
      return CloudSyncError::ProjectStorageLimitReached;
   case ResponseResultCode::TooLarge:
      return CloudSyncError::ProjectStorageLimitReached;
   case ResponseResultCode::Unauthorized:
      return CloudSyncError::Authorization;
   case ResponseResultCode::Forbidden:
      return CloudSyncError::Authorization;
   case ResponseResultCode::NotFound:
      return CloudSyncError::ProjectNotFound;
   case ResponseResultCode::UnexpectedResponse:
      return CloudSyncError::Server;
   case ResponseResultCode::InternalClientError:
      return CloudSyncError::ClientFailure;
   case ResponseResultCode::UnknownError:
      return CloudSyncError::DataUploadFailed;
   }

   return CloudSyncError::DataUploadFailed;
}

} // namespace audacity::cloud::audiocom::sync
