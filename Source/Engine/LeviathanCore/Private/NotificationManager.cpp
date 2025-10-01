#include "NotificationManager.h"

void Core::NotificationManager::SendNotification(const NotificationData& Notification)
{
	NotificationDelegate.Execute(Notification);
}

void Core::NotificationManager::AddNotificationListenerFunction(void(*Function)(const NotificationData&))
{
	NotificationDelegate.AddFunction(Function);
}

void Core::NotificationManager::AddNotificationListenerLambda(void(*Lambda)(const NotificationData&))
{
	NotificationDelegate.AddLambda(Lambda);
}

void Core::NotificationManager::RemoveNotificationListenerFunction(void(*Function)(const NotificationData&))
{
	NotificationDelegate.RemoveFunction(Function);
}

void Core::NotificationManager::RemoveNotificationListenerLambda(void(*Lambda)(const NotificationData&))
{
	NotificationDelegate.RemoveLambda(Lambda);
}

void Core::NotificationManager::ClearNotificationListeners()
{
	NotificationDelegate.Clear();
}
