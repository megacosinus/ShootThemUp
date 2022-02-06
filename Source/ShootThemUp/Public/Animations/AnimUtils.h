#pragma once

class AnimUtils
{
public:
    // Шаблонная функция (вообще, название немного некорректное, т.к. она достаёт только первый нотифай):
    template <typename T> static T* FindNotifyByClass(UAnimSequenceBase* Animation) // статичная, т.к. не зависит от экземпляра класса
    {
        if (!Animation)
            return nullptr;
        // берём массив анимационных ивентов:
        const auto NotifyEvents = Animation->Notifies;
        for (auto NotifyEvent : NotifyEvents)
        {
            // чтобы узнать, является ли данный нотифай нашим нотифаем EquipFinished
            // мы попытаемся преобразовать данный нотифай к нашему типу:
            auto AnimNotify = Cast<T>(NotifyEvent.Notify);
            // если преобразование прошло успешно, то именно этот нотифай нам и нужен
            if (AnimNotify)
            {
                return AnimNotify;
            }
        }
        return nullptr;
    }
    // Конец шаблонной функции
};