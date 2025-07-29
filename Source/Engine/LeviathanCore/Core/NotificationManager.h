#pragma once

#include "ProgrammingTools/Delegate.h"

namespace Core
{
	// TODO: How will notification types from other modules work?

	// NOTE: Add a notification payload structure for each notification type containing data for the type of notifcation
	struct NotificationPayload_GameControllerConnected
	{
	};

	struct NotificationPayload_GameControllerDisconnected
	{
	};

	// NOTE: Add an entry to the enum for each notification type
	enum class NotificationType : uint8_t
	{
		GameControllerConnected,
		GameControllerDisconnected,
		MAX
	};

	struct NotificationData
	{
		NotificationType Type = NotificationType::MAX;

		// NOTE: Add a notifcation payload instance to the notification payload union for each notification type
		union NotificationPayload
		{
			NotificationPayload_GameControllerConnected GameControllerConnected;
			NotificationPayload_GameControllerDisconnected GameControllerDisconnected;
		}Payload;
	};

	DECLARE_MULTI_DELEGATE_OneParam(NotificationDelegateType, const NotificationData& /* Notification */);

	class NotificationManager
	{
	public:
		Core::NotificationDelegateType NotificationDelegate = {};

	public:
		void SendNotification(const Core::NotificationData& Notification);
		void AddNotificationListenerFunction(void(*Function)(const Core::NotificationData&));
		void AddNotificationListenerLambda(void(*Lambda)(const Core::NotificationData&));

		template<class T, void(T::*Method)(const Core::NotificationData&)>
		void AddNotificationListenerMethod(T* ListeningObjectInstancePtr)
		{
			NotificationDelegate.AddMethod<T, Method>(ListeningObjectInstancePtr);
		}

		void RemoveNotificationListenerFunction(void(*Function)(const Core::NotificationData&));
		void RemoveNotificationListenerLambda(void(*Lambda)(const Core::NotificationData&));

		template<class T, void(T::* Method)(const Core::NotificationData&)>
		void RemoveNotificationListenerMethod(T* ListeningObjectInstancePtr)
		{
			NotificationDelegate.RemoveMethod<T, Method>(ListeningObjectInstancePtr);
		}

		void ClearNotificationListeners();
	};
}