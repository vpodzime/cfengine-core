/*
  Copyright 2022 Northern.tech AS

  This file is part of CFEngine 3 - written and maintained by Northern.tech AS.

  This program is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; version 3.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA

 To the extent this program is licensed as part of the Enterprise
 versions of CFEngine, the applicable Commercial Open Source License
 (COSL) may apply to this file if you as a licensee so wish it. See
 included file COSL.txt.
*/
#include <libgen.h>             /* dirname() */

#include <alloc.h>
#include <string_lib.h>
#include <logging.h>

#include <loading.h>
#include <generic_agent.h>
#include <eval_context.h>

#include <sliby.h>

struct SlibyPolicy_ {
    Policy *policy;
    EvalContext *ctx;
    GenericAgentConfig *config;
};

SlibyPolicy SlibyLoadPolicy(const char *entry_file, const char *agent_type)
{
    AgentType atype;
    if (StringEqual(agent_type, CF_COMMONC))
    {
        atype = AGENT_TYPE_COMMON;
    }
    else if (StringEqual(agent_type, CF_AGENTC))
    {
        atype = AGENT_TYPE_AGENT;
    }
    else if (StringEqual(agent_type, CF_SERVERC))
    {
        atype = AGENT_TYPE_SERVER;
    }
    else if (StringEqual(agent_type, CF_MONITORC))
    {
        atype = AGENT_TYPE_MONITOR;
    }
    else if (StringEqual(agent_type, CF_EXECC))
    {
        atype = AGENT_TYPE_EXECUTOR;
    }
    else if (StringEqual(agent_type, CF_RUNC))
    {
        atype = AGENT_TYPE_RUNAGENT;
    }
    else if (StringEqual(agent_type, CF_KEYGEN))
    {
        atype = AGENT_TYPE_KEYGEN;
    }
    else if (StringEqual(agent_type, CF_HUBC))
    {
        atype = AGENT_TYPE_HUB;
    }
    else
    {
        Log(LOG_LEVEL_ERR, "Unrecognized agent type '%s'", agent_type);
        return NULL;
    }

    GenericAgentConfig *config = GenericAgentConfigNewDefault(atype, GetTTYInteractive());
    char path_buf[PATH_MAX];
    strncpy(path_buf, entry_file, sizeof(path_buf));
    char * input_dir = dirname(path_buf);
    config->input_file = xstrdup(entry_file);
    config->input_dir = xstrdup(input_dir);

    EvalContext *ctx = EvalContextNew();
    Policy *policy = SelectAndLoadPolicy(config, ctx, false, false);
    if (policy == NULL)
    {
        EvalContextDestroy(ctx);
        GenericAgentConfigDestroy(config);
        return NULL;
    }

    SlibyPolicy sb_policy = xmalloc(sizeof(SlibyPolicy));
    sb_policy->policy = policy;
    sb_policy->ctx = ctx;
    sb_policy->config = config;

    return sb_policy;
};

void SlibyPolicyDestroy(SlibyPolicy policy)
{
    if (policy != NULL)
    {
        EvalContextDestroy(policy->ctx);
        GenericAgentConfigDestroy(policy->config);
        PolicyDestroy(policy->policy);
        free(policy);
    }
}

char *SlibyPolicyGetVariableValue(SlibyPolicy policy, const char *namespace, const char *bundle, const char *var_name)
{
    char *var_ref_str;
    xasprintf(&var_ref_str, "%s:%s.%s", namespace, bundle, var_name);
    VarRef *var_ref = VarRefParse(var_ref_str);
    if (var_ref == NULL)
    {
        Log(LOG_LEVEL_ERR, "Failed to parse variable reference '%s'", var_ref_str);
        free(var_ref_str);
        return NULL;
    }

    DataType type;
    const void *value = EvalContextVariableGet(policy->ctx, var_ref, &type);
    if (type == CF_DATA_TYPE_NONE)
    {
        Log(LOG_LEVEL_ERR, "Failed to get value for variable '%s'", var_ref_str);
        free(var_ref_str);
        VarRefDestroy(var_ref);
        return NULL;
    }
    else if (type == CF_DATA_TYPE_STRING)
    {
        VarRefDestroy(var_ref);
        return xstrdup((char*) value);
    }
    else
    {
        Log(LOG_LEVEL_ERR, "Unsupported variable type");
        VarRefDestroy(var_ref);
        return NULL;
    }
}

char *SlibyPolicyGetControlBodyAttribute(SlibyPolicy policy, const char *body_type, const char *attribute)
{
    AgentType agent_type = policy->config->agent_type;
    if (body_type != NULL)
    {
        /* TODO: body type -> agent_type */
    }

    Seq *constraints = ControlBodyConstraints(policy->policy, agent_type);
    if (constraints == NULL)
    {
        Log(LOG_LEVEL_ERR, "Failed to find constraints");
        return NULL;
    }

    size_t length = SeqLength(constraints);
    for (size_t i = 0; i < length; i++)
    {
        Constraint *cp = SeqAt(constraints, i);

        if (!IsDefinedClass(policy->ctx, cp->classes))
        {
            continue;
        }

        if (StringEqual(cp->lval, attribute))
        {
            VarRef *var_ref = VarRefParseFromScope(cp->lval, "control_executor");
            DataType type;
            const void *value = EvalContextVariableGet(policy->ctx, var_ref, &type);
            VarRefDestroy(var_ref);

            if (type == CF_DATA_TYPE_STRING)
            {
                return xstrdup((char*) value);
            }
            else
            {
                Log(LOG_LEVEL_ERR, "Unsupported attribute type");
                return NULL;
            }
        }
    }

    Log(LOG_LEVEL_ERR, "Failed to find attribute '%s' in body '%s'", attribute, body_type);
    return NULL;
}
