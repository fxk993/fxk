/**
 * @file config.h
 * @brief ����ģ��
 * @author sylar.yin
 * @email 564628276@qq.com
 * @date 2019-05-22
 * @copyright Copyright (c) 2019�� sylar.yin All rights reserved (www.sylar.top)
 */
#ifndef __SYLAR_CONFIG_H__
#define __SYLAR_CONFIG_H__

#include <memory>
#include <string>
#include <sstream>
#include <boost/lexical_cast.hpp>
#include <yaml-cpp/yaml.h>
#include <vector>
#include <list>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <functional>

#include "thread.h"
#include "log.h"
#include "util.h"

namespace sylar {

    /**
     * @brief ���ñ����Ļ���
     */
    class ConfigVarBase {
    public:
        typedef std::shared_ptr<ConfigVarBase> ptr;
        /**
         * @brief ���캯��
         * @param[in] name ���ò�������[0-9a-z_.]
         * @param[in] description ���ò�������
         */
        ConfigVarBase(const std::string& name, const std::string& description = "")
            :m_name(name)
            , m_description(description) {
            std::transform(m_name.begin(), m_name.end(), m_name.begin(), ::tolower);
        }

        /**
         * @brief ��������
         */
        virtual ~ConfigVarBase() {}

        /**
         * @brief �������ò�������
         */
        const std::string& getName() const { return m_name; }

        /**
         * @brief �������ò���������
         */
        const std::string& getDescription() const { return m_description; }

        /**
         * @brief ת���ַ���
         */
        virtual std::string toString() = 0;

        /**
         * @brief ���ַ�����ʼ��ֵ
         */
        virtual bool fromString(const std::string& val) = 0;

        /**
         * @brief �������ò���ֵ����������
         */
        virtual std::string getTypeName() const = 0;
    protected:
        /// ���ò���������
        std::string m_name;
        /// ���ò���������
        std::string m_description;
    };

    /**
     * @brief ����ת��ģ����(F Դ����, T Ŀ������)
     */
    template<class F, class T>
    class LexicalCast {
    public:
        /**
         * @brief ����ת��
         * @param[in] v Դ����ֵ
         * @return ����vת�����Ŀ������
         * @exception �����Ͳ���ת��ʱ�׳��쳣
         */
        T operator()(const F& v) {
            return boost::lexical_cast<T>(v);
        }
    };

    /**
     * @brief ����ת��ģ����Ƭ�ػ�(YAML String ת���� std::vector<T>)
     */
    template<class T>
    class LexicalCast<std::string, std::vector<T> > {
    public:
        std::vector<T> operator()(const std::string& v) {
            YAML::Node node = YAML::Load(v);
            typename std::vector<T> vec;
            std::stringstream ss;
            for (size_t i = 0; i < node.size(); ++i) {
                ss.str("");
                ss << node[i];
                vec.push_back(LexicalCast<std::string, T>()(ss.str()));
            }
            return vec;
        }
    };

    /**
     * @brief ����ת��ģ����Ƭ�ػ�(std::vector<T> ת���� YAML String)
     */
    template<class T>
    class LexicalCast<std::vector<T>, std::string> {
    public:
        std::string operator()(const std::vector<T>& v) {
            YAML::Node node(YAML::NodeType::Sequence);
            for (auto& i : v) {
                node.push_back(YAML::Load(LexicalCast<T, std::string>()(i)));
            }
            std::stringstream ss;
            ss << node;
            return ss.str();
        }
    };

    /**
     * @brief ����ת��ģ����Ƭ�ػ�(YAML String ת���� std::list<T>)
     */
    template<class T>
    class LexicalCast<std::string, std::list<T> > {
    public:
        std::list<T> operator()(const std::string& v) {
            YAML::Node node = YAML::Load(v);
            typename std::list<T> vec;
            std::stringstream ss;
            for (size_t i = 0; i < node.size(); ++i) {
                ss.str("");
                ss << node[i];
                vec.push_back(LexicalCast<std::string, T>()(ss.str()));
            }
            return vec;
        }
    };

    /**
     * @brief ����ת��ģ����Ƭ�ػ�(std::list<T> ת���� YAML String)
     */
    template<class T>
    class LexicalCast<std::list<T>, std::string> {
    public:
        std::string operator()(const std::list<T>& v) {
            YAML::Node node(YAML::NodeType::Sequence);
            for (auto& i : v) {
                node.push_back(YAML::Load(LexicalCast<T, std::string>()(i)));
            }
            std::stringstream ss;
            ss << node;
            return ss.str();
        }
    };

    /**
     * @brief ����ת��ģ����Ƭ�ػ�(YAML String ת���� std::set<T>)
     */
    template<class T>
    class LexicalCast<std::string, std::set<T> > {
    public:
        std::set<T> operator()(const std::string& v) {
            YAML::Node node = YAML::Load(v);
            typename std::set<T> vec;
            std::stringstream ss;
            for (size_t i = 0; i < node.size(); ++i) {
                ss.str("");
                ss << node[i];
                vec.insert(LexicalCast<std::string, T>()(ss.str()));
            }
            return vec;
        }
    };

    /**
     * @brief ����ת��ģ����Ƭ�ػ�(std::set<T> ת���� YAML String)
     */
    template<class T>
    class LexicalCast<std::set<T>, std::string> {
    public:
        std::string operator()(const std::set<T>& v) {
            YAML::Node node(YAML::NodeType::Sequence);
            for (auto& i : v) {
                node.push_back(YAML::Load(LexicalCast<T, std::string>()(i)));
            }
            std::stringstream ss;
            ss << node;
            return ss.str();
        }
    };

    /**
     * @brief ����ת��ģ����Ƭ�ػ�(YAML String ת���� std::unordered_set<T>)
     */
    template<class T>
    class LexicalCast<std::string, std::unordered_set<T> > {
    public:
        std::unordered_set<T> operator()(const std::string& v) {
            YAML::Node node = YAML::Load(v);
            typename std::unordered_set<T> vec;
            std::stringstream ss;
            for (size_t i = 0; i < node.size(); ++i) {
                ss.str("");
                ss << node[i];
                vec.insert(LexicalCast<std::string, T>()(ss.str()));
            }
            return vec;
        }
    };

    /**
     * @brief ����ת��ģ����Ƭ�ػ�(std::unordered_set<T> ת���� YAML String)
     */
    template<class T>
    class LexicalCast<std::unordered_set<T>, std::string> {
    public:
        std::string operator()(const std::unordered_set<T>& v) {
            YAML::Node node(YAML::NodeType::Sequence);
            for (auto& i : v) {
                node.push_back(YAML::Load(LexicalCast<T, std::string>()(i)));
            }
            std::stringstream ss;
            ss << node;
            return ss.str();
        }
    };

    /**
     * @brief ����ת��ģ����Ƭ�ػ�(YAML String ת���� std::map<std::string, T>)
     */
    template<class T>
    class LexicalCast<std::string, std::map<std::string, T> > {
    public:
        std::map<std::string, T> operator()(const std::string& v) {
            YAML::Node node = YAML::Load(v);
            typename std::map<std::string, T> vec;
            std::stringstream ss;
            for (auto it = node.begin();
                it != node.end(); ++it) {
                ss.str("");
                ss << it->second;
                vec.insert(std::make_pair(it->first.Scalar(),
                    LexicalCast<std::string, T>()(ss.str())));
            }
            return vec;
        }
    };

    /**
     * @brief ����ת��ģ����Ƭ�ػ�(std::map<std::string, T> ת���� YAML String)
     */
    template<class T>
    class LexicalCast<std::map<std::string, T>, std::string> {
    public:
        std::string operator()(const std::map<std::string, T>& v) {
            YAML::Node node(YAML::NodeType::Map);
            for (auto& i : v) {
                node[i.first] = YAML::Load(LexicalCast<T, std::string>()(i.second));
            }
            std::stringstream ss;
            ss << node;
            return ss.str();
        }
    };

    /**
     * @brief ����ת��ģ����Ƭ�ػ�(YAML String ת���� std::unordered_map<std::string, T>)
     */
    template<class T>
    class LexicalCast<std::string, std::unordered_map<std::string, T> > {
    public:
        std::unordered_map<std::string, T> operator()(const std::string& v) {
            YAML::Node node = YAML::Load(v);
            typename std::unordered_map<std::string, T> vec;
            std::stringstream ss;
            for (auto it = node.begin();
                it != node.end(); ++it) {
                ss.str("");
                ss << it->second;
                vec.insert(std::make_pair(it->first.Scalar(),
                    LexicalCast<std::string, T>()(ss.str())));
            }
            return vec;
        }
    };

    /**
     * @brief ����ת��ģ����Ƭ�ػ�(std::unordered_map<std::string, T> ת���� YAML String)
     */
    template<class T>
    class LexicalCast<std::unordered_map<std::string, T>, std::string> {
    public:
        std::string operator()(const std::unordered_map<std::string, T>& v) {
            YAML::Node node(YAML::NodeType::Map);
            for (auto& i : v) {
                node[i.first] = YAML::Load(LexicalCast<T, std::string>()(i.second));
            }
            std::stringstream ss;
            ss << node;
            return ss.str();
        }
    };


    /**
     * @brief ���ò���ģ������,�����Ӧ���͵Ĳ���ֵ
     * @details T �����ľ�������
     *          FromStr ��std::stringת����T���͵ķº���
     *          ToStr ��Tת����std::string�ķº���
     *          std::string ΪYAML��ʽ���ַ���
     */
    template<class T, class FromStr = LexicalCast<std::string, T>
        , class ToStr = LexicalCast<T, std::string> >
            class ConfigVar : public ConfigVarBase {
            public:
                typedef RWMutex RWMutexType;
                typedef std::shared_ptr<ConfigVar> ptr;
                typedef std::function<void(const T& old_value, const T& new_value)> on_change_cb;

                /**
                 * @brief ͨ��������,����ֵ,��������ConfigVar
                 * @param[in] name ����������Ч�ַ�Ϊ[0-9a-z_.]
                 * @param[in] default_value ������Ĭ��ֵ
                 * @param[in] description ����������
                 */
                ConfigVar(const std::string& name
                    , const T& default_value
                    , const std::string& description = "")
                    :ConfigVarBase(name, description)
                    , m_val(default_value) {
                }

                /**
                 * @brief ������ֵת����YAML String
                 * @exception ��ת��ʧ���׳��쳣
                 */
                std::string toString() override {
                    try {
                        //return boost::lexical_cast<std::string>(m_val);
                        RWMutexType::ReadLock lock(m_mutex);
                        return ToStr()(m_val);
                    }
                    catch (std::exception& e) {
                        SYLAR_LOG_ERROR(SYLAR_LOG_ROOT()) << "ConfigVar::toString exception "
                            << e.what() << " convert: " << TypeToName<T>() << " to string"
                            << " name=" << m_name;
                    }
                    return "";
                }

                /**
                 * @brief ��YAML String ת�ɲ�����ֵ
                 * @exception ��ת��ʧ���׳��쳣
                 */
                bool fromString(const std::string& val) override {
                    try {
                        setValue(FromStr()(val));
                    }
                    catch (std::exception& e) {
                        SYLAR_LOG_ERROR(SYLAR_LOG_ROOT()) << "ConfigVar::fromString exception "
                            << e.what() << " convert: string to " << TypeToName<T>()
                            << " name=" << m_name
                            << " - " << val;
                    }
                    return false;
                }

                /**
                 * @brief ��ȡ��ǰ������ֵ
                 */
                const T getValue() {
                    RWMutexType::ReadLock lock(m_mutex);
                    return m_val;
                }

                /**
                 * @brief ���õ�ǰ������ֵ
                 * @details ���������ֵ�з����仯,��֪ͨ��Ӧ��ע��ص�����
                 */
                void setValue(const T& v) {
                    {
                        RWMutexType::ReadLock lock(m_mutex);
                        if (v == m_val) {
                            return;
                        }
                        for (auto& i : m_cbs) {
                            i.second(m_val, v);
                        }
                    }
                    RWMutexType::WriteLock lock(m_mutex);
                    m_val = v;
                }

                /**
                 * @brief ���ز���ֵ����������(typeinfo)
                 */
                std::string getTypeName() const override { return TypeToName<T>(); }

                /**
                 * @brief ��ӱ仯�ص�����
                 * @return ���ظûص�������Ӧ��Ψһid,����ɾ���ص�
                 */
                uint64_t addListener(on_change_cb cb) {
                    static uint64_t s_fun_id = 0;
                    RWMutexType::WriteLock lock(m_mutex);
                    ++s_fun_id;
                    m_cbs[s_fun_id] = cb;
                    return s_fun_id;
                }

                /**
                 * @brief ɾ���ص�����
                 * @param[in] key �ص�������Ψһid
                 */
                void delListener(uint64_t key) {
                    RWMutexType::WriteLock lock(m_mutex);
                    m_cbs.erase(key);
                }

                /**
                 * @brief ��ȡ�ص�����
                 * @param[in] key �ص�������Ψһid
                 * @return ������ڷ��ض�Ӧ�Ļص�����,���򷵻�nullptr
                 */
                on_change_cb getListener(uint64_t key) {
                    RWMutexType::ReadLock lock(m_mutex);
                    auto it = m_cbs.find(key);
                    return it == m_cbs.end() ? nullptr : it->second;
                }

                /**
                 * @brief �������еĻص�����
                 */
                void clearListener() {
                    RWMutexType::WriteLock lock(m_mutex);
                    m_cbs.clear();
                }
            private:
                RWMutexType m_mutex;
                T m_val;
                //����ص�������, uint64_t key,Ҫ��Ψһ��һ�������hash
                std::map<uint64_t, on_change_cb> m_cbs;
        };

        /**
         * @brief ConfigVar�Ĺ�����
         * @details �ṩ��ݵķ�������/����ConfigVar
         */
        class Config {
        public:
            typedef std::unordered_map<std::string, ConfigVarBase::ptr> ConfigVarMap;
            typedef RWMutex RWMutexType;

            /**
             * @brief ��ȡ/������Ӧ�����������ò���
             * @param[in] name ���ò�������
             * @param[in] default_value ����Ĭ��ֵ
             * @param[in] description ��������
             * @details ��ȡ������Ϊname�����ò���,�������ֱ�ӷ���
             *          ���������,�����������ò���default_value��ֵ
             * @return ���ض�Ӧ�����ò���,������������ڵ������Ͳ�ƥ���򷵻�nullptr
             * @exception ��������������Ƿ��ַ�[^0-9a-z_.] �׳��쳣 std::invalid_argument
             */
            template<class T>
            static typename ConfigVar<T>::ptr Lookup(const std::string& name,
                const T& default_value, const std::string& description = "") {
                RWMutexType::WriteLock lock(GetMutex());
                auto it = GetDatas().find(name);
                if (it != GetDatas().end()) {
                    auto tmp = std::dynamic_pointer_cast<ConfigVar<T>>(it->second);
                    if (tmp) {
                        SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << "Lookup name=" << name << " exists";
                        return tmp;
                    }
                    else {
                        SYLAR_LOG_ERROR(SYLAR_LOG_ROOT()) << "Lookup name=" << name << " exists but type not "
                            << TypeToName<T>() << " real_type=" << it->second->getTypeName()
                            << " " << it->second->toString();
                        return nullptr;
                    }
                }

                if (name.find_first_not_of("abcdefghikjlmnopqrstuvwxyz._012345678")
                    != std::string::npos) {
                    SYLAR_LOG_ERROR(SYLAR_LOG_ROOT()) << "Lookup name invalid " << name;
                    throw std::invalid_argument(name);
                }

                typename ConfigVar<T>::ptr v(new ConfigVar<T>(name, default_value, description));
                GetDatas()[name] = v;
                return v;
            }

            /**
             * @brief �������ò���
             * @param[in] name ���ò�������
             * @return �������ò�����Ϊname�����ò���
             */
            template<class T>
            static typename ConfigVar<T>::ptr Lookup(const std::string& name) {
                RWMutexType::ReadLock lock(GetMutex());
                auto it = GetDatas().find(name);
                if (it == GetDatas().end()) {
                    return nullptr;
                }
                return std::dynamic_pointer_cast<ConfigVar<T>>(it->second);
            }

            /**
             * @brief ʹ��YAML::Node��ʼ������ģ��
             */
            static void LoadFromYaml(const YAML::Node& root);

            /**
             * @brief ����path�ļ�������������ļ�
             */
            static void LoadFromConfDir(const std::string& path, bool force = false);

            /**
             * @brief �������ò���,�������ò����Ļ���
             * @param[in] name ���ò�������
             */
            static ConfigVarBase::ptr LookupBase(const std::string& name);

            /**
             * @brief ��������ģ����������������
             * @param[in] cb ������ص�����
             */
            static void Visit(std::function<void(ConfigVarBase::ptr)> cb);
        private:

            /**
             * @brief �������е�������
             */
            static ConfigVarMap& GetDatas() {
                static ConfigVarMap s_datas;
                return s_datas;
            }

            /**
             * @brief �������RWMutex
             */
            static RWMutexType& GetMutex() {
                static RWMutexType s_mutex;
                return s_mutex;
            }
        };

}

#endif