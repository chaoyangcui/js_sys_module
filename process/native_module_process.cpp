/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <vector>
#include <grp.h>
#include <sys/types.h>
#include <unistd.h>

#include "js_childprocess.h"
#include "js_process.h"
#include "securec.h"
#include "utils/log.h"
namespace OHOS::Js_sys_module::Process {
    static napi_value DealType(napi_env env, napi_value args[], size_t argsSize)
    {
        if (args[0] != nullptr) {
            napi_valuetype valueType = napi_undefined;
            NAPI_CALL(env, napi_typeof(env, args[0], &valueType));
            NAPI_ASSERT(env, valueType == napi_string, "Wrong argument typr. String expected");
        } else {
            HILOG_ERROR("command is null");
            NAPI_CALL(env, napi_throw_error(env, "", "command is empty"));
        }

        std::vector<std::string> keyStr = {"timeout", "killSignal", "maxBuffer"};

        if (args[1] == nullptr) {
            return nullptr;
        }
        size_t size = keyStr.size();
        for (size_t i = 0; i < size; i++) {
            napi_valuetype propertyType = napi_undefined;
            napi_value property = nullptr;
            NAPI_CALL(env, napi_get_named_property(env, args[1], keyStr[i].c_str(), &property));
            switch (i) {
                case 0:
                    {
                        NAPI_CALL(env, napi_typeof(env, property, &propertyType));
                        NAPI_ASSERT(env, propertyType == napi_number || propertyType == napi_undefined,
                                    "Wrong timeout argument typr. Number expected");
                        int timeout = 0;
                        NAPI_CALL(env, napi_get_value_int32(env, property, &timeout));
                        if (timeout < 0) {
                            NAPI_CALL(env, napi_throw_error(env, "", "options timeout is lessthen zero"));
                        }
                        break;
                    }
                case 1:
                    NAPI_CALL(env, napi_typeof(env, property, &propertyType));
                    NAPI_ASSERT(env, propertyType == napi_string || propertyType == napi_number
                                || propertyType == napi_undefined,
                                "Wrong KillSignal argument typr. String or number expected");
                    break;
                case 2:
                    NAPI_CALL(env, napi_typeof(env, property, &propertyType));
                    NAPI_ASSERT(env, propertyType == napi_number || propertyType == napi_undefined,
                                "Wrong maxBuffer argument typr. Number expected");
                    break;
                default:
                    break;
            }
        }
        return nullptr;
    }

    static napi_value ChildProcessConstructor(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        void* data = nullptr;
        size_t argc = 2;
        napi_value args[2] = { nullptr };
        NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &thisVar, &data));
        DealType(env, args, argc);
        auto objectInfo = new ChildProcess(env);
        objectInfo->InitOptionsInfo(args[1]);
        objectInfo->Spawn(args[0]);
        NAPI_CALL(env, napi_wrap(
            env, thisVar, objectInfo,
            [](napi_env env, void* data, void* hint) {
                auto objectInfo = (ChildProcess*)data;
                if (objectInfo != nullptr) {
                    delete objectInfo;
                }
            },
            nullptr, nullptr));
        return thisVar;
    }

    static napi_value Wait(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr));
        ChildProcess* object = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, reinterpret_cast<void**>(&object)));
        napi_value result = object->Wait();
        return result;
    }

    static napi_value GetOutput(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr));
        ChildProcess* object = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, reinterpret_cast<void**>(&object)));
        napi_value result = object->GetOutput();
        return result;
    }

    static napi_value Close(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr));
        ChildProcess* object = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, reinterpret_cast<void**>(&object)));
        object->Close();
        napi_value result = nullptr;
        NAPI_CALL(env, napi_get_undefined(env, &result));
        return result;
    }

    static napi_value GetErrorOutput(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr));
        ChildProcess* object = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, reinterpret_cast<void**>(&object)));
        napi_value result = object->GetErrorOutput();
        return result;
    }

    static napi_value Kill(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        size_t requireArgc = 1;
        size_t argc = 1;
        napi_value args = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, &argc, &args, &thisVar, nullptr));
        NAPI_ASSERT(env, argc >= requireArgc, "Wrong number of arguments");
        napi_valuetype valuetype;
        NAPI_CALL(env, napi_typeof(env, args, &valuetype));
        if ((valuetype != napi_valuetype::napi_number) && (valuetype != napi_valuetype::napi_string)) {
            napi_throw_error(env, nullptr, "The parameter type is incorrect");
        }
        ChildProcess* object = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, reinterpret_cast<void**>(&object)));
        object->Kill(args);
        napi_value result = nullptr;
        NAPI_CALL(env, napi_get_undefined(env, &result));
        return result;
    }

    static napi_value GetKilled(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr));
        ChildProcess* object = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, reinterpret_cast<void**>(&object)));
        napi_value result = object->GetKilled();
        return result;
    }

    static napi_value Getpid(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr));
        ChildProcess* object = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, reinterpret_cast<void**>(&object)));
        napi_value result = object->Getpid();
        return result;
    }

    static napi_value Getppid(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr));
        ChildProcess* object = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, reinterpret_cast<void**>(&object)));
        napi_value result = object->Getppid();
        return result;
    }

    static napi_value GetExitCode(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr));
        ChildProcess* object = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, reinterpret_cast<void**>(&object)));
        napi_value result = object->GetExitCode();
        return result;
    }

    static napi_value RunCommand(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        size_t argc = 2;
        napi_value args[2] = { nullptr };
        NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &thisVar, nullptr));
        const char* childProcessClassName = "ChildProcess";
        napi_value childProcessClass = nullptr;
        static napi_property_descriptor childProcessDesc[] = {
            DECLARE_NAPI_FUNCTION("close", Close),
            DECLARE_NAPI_FUNCTION("kill", Kill),
            DECLARE_NAPI_FUNCTION("getOutput", GetOutput),
            DECLARE_NAPI_FUNCTION("getErrorOutput", GetErrorOutput),
            DECLARE_NAPI_FUNCTION("wait", Wait),
            DECLARE_NAPI_GETTER("killed", GetKilled),
            DECLARE_NAPI_GETTER("pid", Getpid),
            DECLARE_NAPI_GETTER("ppid", Getppid),
            DECLARE_NAPI_GETTER("exitCode", GetExitCode),
        };
        NAPI_CALL(env, napi_define_class(env, childProcessClassName, strlen(childProcessClassName),
                                         ChildProcessConstructor, nullptr,
                                         sizeof(childProcessDesc) / sizeof(childProcessDesc[0]), childProcessDesc,
                                         &childProcessClass));
        napi_value result = nullptr;
        NAPI_CALL(env, napi_new_instance(env, childProcessClass, argc, args, &result));
        return result;
    }

    static napi_value GetUid(napi_env env, napi_callback_info info)
    {
        Process object(env);
        return object.GetUid();
    }

    static napi_value GetGid(napi_env env, napi_callback_info info)
    {
        Process object(env);
        return object.GetGid();
    }

    static napi_value GetEUid(napi_env env, napi_callback_info info)
    {
        Process object(env);
        return object.GetEUid();
    }

    static napi_value GetEGid(napi_env env, napi_callback_info info)
    {
        Process object(env);
        return object.GetEGid();
    }

    static napi_value GetGroups(napi_env env, napi_callback_info info)
    {
        Process object(env);
        return object.GetGroups();
    }

    static napi_value GetPid(napi_env env, napi_callback_info info)
    {
        Process object(env);
        return object.GetPid();
    }

    static napi_value GetPpid(napi_env env, napi_callback_info info)
    {
        Process object(env);
        return object.GetPpid();
    }

    static napi_value Chdir(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        size_t requireArgc = 1;
        size_t argc = 1;
        napi_value args = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, &argc, &args, &thisVar, nullptr));
        NAPI_ASSERT(env, argc >= requireArgc, "Wrong nuamber of arguments");
        napi_valuetype valuetype;
        NAPI_CALL(env, napi_typeof(env, args, &valuetype));
        NAPI_ASSERT(env, valuetype == napi_string, "Wrong argument type. String expected");
        Process object(env);
        object.Chdir(args);
        napi_value result = nullptr;
        NAPI_CALL(env, napi_get_undefined(env, &result));
        return result;
    }

    static napi_value Abort(napi_env env, napi_callback_info info)
    {
        Process object(env);
        object.Abort();
        napi_value res = nullptr;
        NAPI_CALL(env, napi_get_undefined(env, &res));
        return res;
    }

    static napi_value Cwd(napi_env env, napi_callback_info info)
    {
        Process object(env);
        return object.Cwd();
    }

    static napi_value Exit(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        size_t argc = 1;
        napi_value args = nullptr;
        napi_get_cb_info(env, info, &argc, &args, &thisVar, nullptr);
        napi_valuetype valuetype;
        NAPI_CALL(env, napi_typeof(env, args, &valuetype));
        NAPI_ASSERT(env, valuetype == napi_number, "Wrong argument type.number error");
        Process object(env);
        object.Exit(args);
        napi_value res = nullptr;
        NAPI_CALL(env, napi_get_undefined(env, &res));
        return res;
    }
    static napi_value On(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        size_t requireArgc = 2;
        size_t argc = 2;
        napi_value args[2] = { nullptr };
        NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &thisVar, nullptr));
        NAPI_ASSERT(env, argc >= requireArgc, "Wrong number of arguments");
        napi_valuetype valuetype0;
        NAPI_CALL(env, napi_typeof(env, args[0], &valuetype0));
        if (valuetype0 != napi_valuetype::napi_string) {
            bool flag = false;
            napi_value result = nullptr;
            NAPI_CALL(env, napi_get_boolean(env, flag, &result));
            return result;
        }
        napi_valuetype valuetype1;
        NAPI_CALL(env, napi_typeof(env, args[1], &valuetype1));
        Process object(env);
        object.On(args[0], args[1]);
        napi_value res = nullptr;
        NAPI_CALL(env, napi_get_undefined(env, &res));
        return res;
    }

    static napi_value Off(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        size_t argc = 1;
        napi_value args = nullptr;
        napi_get_cb_info(env, info, &argc, &args, &thisVar, nullptr);
        Process object(env);
        napi_value result = object.Off(args);
        return result;
    }

    static napi_value Uptime(napi_env env, napi_callback_info info)
    {
        Process object(env);
        return object.Uptime();
    }

    static napi_value KillSig(napi_env env, napi_callback_info info)
    {
        size_t argc = 2;
        napi_value argv[2] = {0};
        napi_value thisVar = nullptr;
        void* data = nullptr;
        napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
        Process object(env);
        napi_value result = nullptr;
        result = object.Kill(argv[0], argv[1]);
        return result;
    }

    static napi_value Init(napi_env env, napi_value exports)
    {
        napi_property_descriptor desc[] = {
            DECLARE_NAPI_FUNCTION("runCmd", RunCommand),
            DECLARE_NAPI_GETTER("getUid", GetUid),
            DECLARE_NAPI_GETTER("getGid", GetGid),
            DECLARE_NAPI_GETTER("getEuid", GetEUid),
            DECLARE_NAPI_GETTER("getEgid", GetEGid),
            DECLARE_NAPI_GETTER("getGroups", GetGroups),
            DECLARE_NAPI_GETTER("getPid", GetPid),
            DECLARE_NAPI_GETTER("getPpid", GetPpid),
            DECLARE_NAPI_FUNCTION("uptime", Uptime),
            DECLARE_NAPI_FUNCTION("kill", KillSig),
            DECLARE_NAPI_FUNCTION("chdir", Chdir),
            DECLARE_NAPI_FUNCTION("abort", Abort),
            DECLARE_NAPI_FUNCTION("cwd", Cwd),
            DECLARE_NAPI_FUNCTION("on", On),
            DECLARE_NAPI_FUNCTION("off", Off),
            DECLARE_NAPI_FUNCTION("exit", Exit),
        };
        NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc));
        return exports;
    }

    static napi_module processModule = {
        .nm_version = 1,
        .nm_flags = 0,
        .nm_filename = nullptr,
        .nm_register_func = Init,
        .nm_modname = "process",
        .nm_priv = ((void*)0),
        .reserved = { 0 },
    };

    extern "C" __attribute__ ((constructor)) void RegisterModule()
    {
        napi_module_register(&processModule);
    }
} // namespace