#pragma once

// Maps each visualiser case to chapter hints for MTH3007 / MTH3001 (edit CaseNoteLinks.cpp to match your notes).

// Multi-line UTF-8 string; nullptr if unknown (should not happen for registered cases).
const char* noteRefsForCaseName(const char* caseNameUtf8);
