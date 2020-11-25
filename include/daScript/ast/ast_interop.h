#pragma once

#include "daScript/ast/ast.h"
#include "daScript/ast/ast_visitor.h"
#include "daScript/simulate/interop.h"

namespace das
{
    class ExternalFnBase : public BuiltInFunction {
    public:
        ExternalFnBase(const char * name, const char * cppName)
            : BuiltInFunction(name, cppName) {
            callBased = true;
        };
    };

    template  <typename FuncT, FuncT fn, typename SimNodeT, typename FuncArgT>
    class ExternalFn : public ExternalFnBase {
        static_assert ( is_base_of<SimNode_CallBase, SimNodeT>::value, "only call-based nodes allowed" );
    public:
        __forceinline ExternalFn(const char * name, const ModuleLibrary & lib, const char * cppName = nullptr)
        : ExternalFnBase(name,cppName) {
            using FunctionTrait = function_traits<FuncArgT>;
            const int nargs = tuple_size<typename FunctionTrait::arguments>::value;
            using Indices = make_index_sequence<nargs>;
            using Arguments = typename FunctionTrait::arguments;
            using Result  = typename FunctionTrait::return_type;
            constructExternal(makeArgs<Result,Arguments>(lib, Indices()));
        }
        virtual SimNode * makeSimNode ( Context & context, const vector<ExpressionPtr> & ) override {
            const char * fnName = context.code->allocateName(this->name);
            return context.code->makeNode<SimNodeT>(at, fnName);
        }
    };

    template  <InteropFunction func, typename RetT, typename ...Args>
    class InteropFn : public BuiltInFunction {
    public:
        __forceinline InteropFn(const char * name, const ModuleLibrary & lib, const char * cppName = nullptr)
            : BuiltInFunction(name,cppName) {
            this->callBased = true;
            this->interopFn = true;
            constructInterop(makeBuiltinArgs<RetT, Args...>(lib));
        }
        virtual SimNode * makeSimNode ( Context & context, const vector<ExpressionPtr> & ) override {
            const char * fnName = context.code->allocateName(this->name);
            return context.code->makeNode<SimNode_InteropFuncCall<func>>(BuiltInFunction::at,fnName);
        }
    };

    struct defaultTempFn {
        defaultTempFn() = default;
        defaultTempFn ( bool args, bool impl, bool result, bool econst )
            : tempArgs(args), implicitArgs(impl), tempResult(result), explicitConstArgs(econst) {}
        __noinline bool operator () ( Function * fn ) {
            if ( tempArgs || implicitArgs ) {
                for ( auto & arg : fn->arguments ) {
                    if ( arg->type->isTempType() ) {
                        arg->type->temporary = tempArgs;
                        arg->type->implicit = implicitArgs;
                        arg->type->explicitConst = explicitConstArgs;
                    }
                }
            }
            if ( tempResult ) {
                if ( fn->result->isTempType() ) {
                    fn->result->temporary = true;
                }
            }
            return true;
        }
        bool tempArgs = false;
        bool implicitArgs = true;
        bool tempResult = false;
        bool explicitConstArgs = false;
    };

    struct permanentArgFn : defaultTempFn {
        permanentArgFn() : defaultTempFn(false,false,false,false) {}
    };

    struct temporaryArgFn : defaultTempFn {
        temporaryArgFn() : defaultTempFn(true,false,false,false) {}
    };

    struct explicitConstArgFn : defaultTempFn {
        explicitConstArgFn() : defaultTempFn(false,true,false,true) {}
    };

    void addExternFunc(Module& mod, const FunctionPtr & fx, bool isCmres, SideEffects seFlags);

    template <typename FuncT, FuncT fn, template <typename FuncTT, FuncTT fnt> class SimNodeT = SimNode_ExtFuncCall, typename QQ = defaultTempFn>
    inline auto addExtern ( Module & mod, const ModuleLibrary & lib, const char * name, SideEffects seFlags,
                                  const char * cppName = nullptr, QQ && tempFn = QQ() ) {
        auto fnX = make_smart<ExternalFn<FuncT, fn, SimNodeT<FuncT, fn>, FuncT>>(name, lib, cppName);
        tempFn(fnX.get());
        addExternFunc(mod, fnX, SimNodeT<FuncT, fn>::IS_CMRES, seFlags);
        return fnX;
    }

    template <typename FuncArgT, typename FuncT, FuncT fn, template <typename FuncTT, FuncTT fnt> class SimNodeT = SimNode_ExtFuncCall>
    inline auto addExternEx ( Module & mod, const ModuleLibrary & lib, const char * name, SideEffects seFlags,
                                  const char * cppName = nullptr ) {
        auto fnX = make_smart<ExternalFn<FuncT, fn, SimNodeT<FuncT, fn>, FuncArgT>>(name, lib, cppName);
        addExternFunc(mod, fnX, SimNodeT<FuncT, fn>::IS_CMRES, seFlags);
        return fnX;
    }

    template <InteropFunction func, typename RetT, typename ...Args>
    inline auto addInterop ( Module & mod, const ModuleLibrary & lib, const char * name, SideEffects seFlags,
                                   const char * cppName = nullptr ) {
        auto fnX = make_smart<InteropFn<func, RetT, Args...>>(name, lib, cppName);
        addExternFunc(mod, fnX, true, seFlags);
        return fnX;
    }
}

