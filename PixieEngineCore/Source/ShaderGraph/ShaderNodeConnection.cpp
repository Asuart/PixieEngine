#include "pch.h"
#include "ShaderNodeConnection.h"
#include "ShaderNodeIO.h"

ShaderNodeConnection::ShaderNodeConnection(ShaderNodeOutput& _from, ShaderNodeInput& _to) :
	from(_from), to(_to) {}
