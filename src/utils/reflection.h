#pragma once
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <json.hpp>
#include <type_traits>
#include <typeindex>
#include <functional>

namespace Cardsity
{
    namespace Reflection
    {
        namespace Registration
        {
            template <class C> struct exists : std::false_type
            {
            };
        } // namespace Registration
        namespace internal
        {
            class ReflectedClassBase;
            inline std::map<std::type_index, ReflectedClassBase> reflectedClasses;

            class Any
            {
              private:
                struct DataBase
                {
                };
                template <typename T> struct Data : DataBase
                {
                    T data;
                    Data(T data) : data(data)
                    {
                    }
                };
                std::shared_ptr<DataBase> data;

              public:
                Any()
                {
                }
                template <typename T> Any(T &&data) : data(std::make_shared<Data<T>>(std::forward<T>(data)))
                {
                }
                template <typename T> auto &get()
                {
                    auto rtn = std::static_pointer_cast<Data<T>>(data);
                    return rtn->data;
                }
            };

            class ReflectedProperty
            {
                std::string name;
                Any memberPtr;

              public:
                std::function<void(nlohmann::json &j, Any clazz)> toJson;
                std::function<void(const nlohmann::json &j, Any clazz)> fromJson;

                ReflectedProperty()
                {
                }
                template <class C, typename T> ReflectedProperty(T C::*member, const std::string &name) : name(name)
                {
                    this->memberPtr = member;
                    toJson = [=](nlohmann::json &j, Any clazz) { j[name] = clazz.get<const C &>().*member; };
                    fromJson = [=](const nlohmann::json &j, Any clazz) { j[name].get_to(clazz.get<C &>().*member); };
                }
                template <class C, typename T, typename GetLambda>
                ReflectedProperty(T C::*member, const std::string &name, GetLambda customGetter) : name(name)
                {
                    this->memberPtr = member;

                    toJson = [=](nlohmann::json &j, Any clazz) {
                        j[name] = customGetter(clazz.get<const C &>().*member);
                    };
                    fromJson = [=](const nlohmann::json &j, Any clazz) { j[name].get_to(clazz.get<C &>().*member); };
                }
                template <class C, typename T, typename GetLambda, typename SetLambda>
                ReflectedProperty(T C::*member, const std::string &name, GetLambda customGetter, SetLambda customSetter)
                    : name(name)
                {
                    this->memberPtr = member;

                    if constexpr (!std::is_same<GetLambda, bool>::value)
                    {
                        toJson = [=](nlohmann::json &j, Any clazz) {
                            j[name] = customGetter(clazz.get<const C &>().*member);
                        };
                    }
                    if constexpr (!std::is_same<SetLambda, bool>::value)
                    {
                        fromJson = [=](const nlohmann::json &j, Any clazz) {
                            j[name].get_to(customSetter(clazz.get<C &>().*member));
                        };
                    }
                }
            };
            class ReflectedClassBase
            {
              protected:
                std::vector<internal::ReflectedProperty> properties;
                std::string name;

              public:
                ReflectedClassBase()
                {
                }
                ReflectedClassBase(const std::string &name) : name(name)
                {
                }
                template <class C> void toJson(nlohmann::json &j, const C &clazz)
                {
                    j["name"] = name;
                    nlohmann::json data;
                    for (auto property : properties)
                    {
                        property.toJson(data, clazz);
                    }
                    j["data"] = data;
                }
                template <class C> void fromJson(const nlohmann::json &j, C &clazz)
                {
                    if (name != j["name"])
                        throw std::runtime_error("Invalid cast");

                    const nlohmann::json &data = j["data"];

                    for (auto property : properties)
                    {
                        property.fromJson(data, clazz);
                    }
                }
            };
            template <class C> class ReflectedClass : ReflectedClassBase
            {
              public:
                using internal::ReflectedClassBase::ReflectedClassBase;

                operator bool()
                {
                    internal::reflectedClasses[typeid(C)] = *this;
                    return true;
                }
                template <typename T> auto &property(T C::*memberPtr, const std::string &name)
                {
                    this->properties.push_back({memberPtr, name});
                    return *this;
                }
                template <typename T, typename GetLambda>
                auto &property(T C::*memberPtr, const std::string &name, GetLambda customGetter)
                {
                    this->properties.push_back({memberPtr, name, customGetter});
                    return *this;
                }
                template <typename T, typename GetLambda, typename SetLambda>
                auto &property(T C::*memberPtr, const std::string &name, GetLambda customGetter, SetLambda customSetter)
                {
                    this->properties.push_back({memberPtr, name, customGetter, customSetter});
                    return *this;
                }
            };
        } // namespace internal
    }     // namespace Reflection
} // namespace Cardsity

#define REGISTER namespace Cardsity::Reflection::Registration

#define class_(clazz)                                                                                                  \
    template <> struct exists<clazz> : std::true_type                                                                  \
    {                                                                                                                  \
    };                                                                                                                 \
    inline const auto clazz##_regisration = (bool)internal::ReflectedClass<clazz>(#clazz)

#define class__(clazz, name)                                                                                           \
    template <> struct exists<clazz> : std::true_type                                                                  \
    {                                                                                                                  \
    };                                                                                                                 \
    inline const auto name##_regisration = (bool)internal::ReflectedClass<clazz>(#clazz)

#define FINISH

namespace nlohmann
{
    template <typename T, std::enable_if_t<Cardsity::Reflection::Registration::exists<T>::value> * = nullptr>
    void to_json(json &j, const T &obj)
    {
        return Cardsity::Reflection::internal::reflectedClasses[typeid(T)].toJson(j, obj);
    }
    template <typename T, std::enable_if_t<Cardsity::Reflection::Registration::exists<T>::value> * = nullptr>
    void from_json(const json &j, T &obj)
    {
        return Cardsity::Reflection::internal::reflectedClasses[typeid(T)].fromJson(j, obj);
    }
} // namespace nlohmann