---
spec_id: REQ-SESSION-001
title: "Requirements Elicitation Session Notes Template - AES5-2018"
version: "1.0.0"
date: "2025-11-06"
status: "template"
phase: "02-requirements"
stakeholder_requirements: ["STR-FUNC-001", "STR-FUNC-002", "STR-FUNC-003", "STR-QUAL-001", "STR-QUAL-002", "STR-QUAL-003", "STR-QUAL-004", "STR-CONST-001", "STR-CONST-002"]
elicitation_framework: "8-dimension"
priority: "critical"
standards_compliance: ["ISO/IEC/IEEE-29148:2018", "AES5-2018"]
---

# Requirements Elicitation Session Notes Template

## Session Information

**Session ID**: REQ-SESSION-[DATE]-[NUMBER]  
**Date**: [YYYY-MM-DD]  
**Time**: [HH:MM - HH:MM Timezone]  
**Duration**: [X hours Y minutes]  
**Session Type**: [ ] Initial Elicitation [ ] Follow-up [ ] Validation [ ] Clarification

### Participants

| Name | Role | Organization | Contact |
|------|------|--------------|---------|
| [Name] | [Role] | [Org] | [Email] |
| [Name] | [Role] | [Org] | [Email] |
| [Name] | [Role] | [Org] | [Email] |

### Session Objectives

**Primary Focus**: [Stakeholder requirement being elicited]  
**Dimensions Covered**: [ ] Functional [ ] Boundaries [ ] Errors [ ] Performance [ ] Security [ ] Compliance [ ] Integration [ ] Priority

## Requirements Being Elicited

### [Requirement ID] - [Requirement Title]

**Stakeholder Requirement**: [Original STR-XXX requirement text]  
**AES5-2018 Reference**: [Clause reference]  
**Priority**: [ ] Critical (P0) [ ] High (P1) [ ] Medium (P2) [ ] Low (P3)

---

## Elicitation Results by Dimension

### Dimension 1: Functional Behavior

#### Questions Asked
1. [Question text]
2. [Question text]
3. [Question text]

#### Stakeholder Responses

**Q1 Response**:  
> [Stakeholder direct quote or paraphrased response]

**Key Points**:
- [Important detail 1]
- [Important detail 2]
- [Important detail 3]

**Follow-up Required**: [ ] Yes [ ] No  
**Follow-up Action**: [Description if yes]

**Q2 Response**:  
> [Stakeholder direct quote or paraphrased response]

**Key Points**:
- [Important detail 1]
- [Important detail 2]

#### Derived System Requirements

**SYS-FUNC-XXX.Y**: [Detailed system requirement derived from responses]

**Rationale**: [Why this requirement was derived]  
**Acceptance Criteria**: [How this requirement will be verified]  
**Dependencies**: [Other requirements this depends on]

---

### Dimension 2: Boundaries & Interfaces

#### Questions Asked
1. [Question text]
2. [Question text]

#### Stakeholder Responses

**Q1 Response**:  
> [Stakeholder response]

**Key Points**:
- [Interface requirement 1]
- [Interface requirement 2]

**Q2 Response**:  
> [Stakeholder response]

**Key Points**:
- [Boundary constraint 1]
- [Boundary constraint 2]

#### Derived System Requirements

**SYS-INT-XXX.Y**: [Interface requirement]  
**SYS-BOUND-XXX.Y**: [Boundary requirement]

---

### Dimension 3: Error Handling

#### Questions Asked
1. [Question text]
2. [Question text]

#### Stakeholder Responses

**Q1 Response**:  
> [Error handling preference]

**Key Points**:
- [Error condition 1]
- [Recovery mechanism 1]

#### Derived System Requirements

**SYS-ERR-XXX.Y**: [Error handling requirement]

---

### Dimension 4: Performance

#### Questions Asked
1. [Question text]
2. [Question text]

#### Stakeholder Responses

**Performance Requirements Identified**:

| Metric | Requirement | Rationale |
|--------|-------------|-----------|
| Latency | [Value] | [Business justification] |
| Throughput | [Value] | [Business justification] |
| Resource Usage | [Value] | [Business justification] |

#### Derived System Requirements

**SYS-PERF-XXX.Y**: [Performance requirement]

---

### Dimension 5: Security

#### Questions Asked
1. [Question text]

#### Stakeholder Responses

**Security Considerations**:
- [Security requirement or "Not applicable"]
- [Access control requirement]

#### Derived System Requirements

**SYS-SEC-XXX.Y**: [Security requirement or "N/A - no security requirements identified"]

---

### Dimension 6: Compliance

#### Questions Asked
1. [Question text]
2. [Question text]

#### Stakeholder Responses

**AES5-2018 Compliance Requirements**:

| AES5-2018 Clause | Compliance Requirement | Verification Method |
|------------------|----------------------|-------------------|
| [Clause X.Y] | [Requirement] | [How to verify] |
| [Clause X.Y] | [Requirement] | [How to verify] |

#### Derived System Requirements

**SYS-COMP-XXX.Y**: [Compliance requirement]

---

### Dimension 7: Integration

#### Questions Asked
1. [Question text]
2. [Question text]

#### Stakeholder Responses

**Integration Dependencies**:
- [System/component this must integrate with]
- [Integration constraint or requirement]

**Integration Approach Preferred**:
- [ ] Tight coupling
- [ ] Loose coupling  
- [ ] Plugin architecture
- [ ] Service-oriented
- [ ] Other: [specify]

#### Derived System Requirements

**SYS-INT-XXX.Y**: [Integration requirement]

---

### Dimension 8: Prioritization

#### Questions Asked
1. [Question text]
2. [Question text]

#### Stakeholder Responses

**Business Priority**: [ ] Critical [ ] High [ ] Medium [ ] Low

**Priority Rationale**: [Why this priority level]

**Trade-off Acceptability**:
- [Acceptable trade-off 1]
- [Acceptable trade-off 2]
- [Non-negotiable aspect 1]

**Implementation Timeline Preference**:
- [ ] Phase 1 (MVP)
- [ ] Phase 2 (Enhancement)
- [ ] Phase 3 (Future)

#### Derived System Requirements

**Priority Classification**: [Final priority with rationale]  
**Implementation Phase**: [Recommended implementation timing]

---

## Session Summary

### Key Insights
1. [Major insight from session]
2. [Important clarification gained]
3. [Unexpected requirement discovered]

### Decisions Made
1. [Decision 1 with rationale]
2. [Decision 2 with rationale]

### Outstanding Questions
1. [Question requiring follow-up]
2. [Clarification needed from other stakeholders]

### Conflicts/Issues Identified
1. [Conflict between stakeholder needs]
2. [Technical constraint issue]

### Next Steps

| Action Item | Owner | Due Date | Priority |
|-------------|-------|----------|----------|
| [Action description] | [Person] | [Date] | [Priority] |
| [Action description] | [Person] | [Date] | [Priority] |

---

## Artifacts Generated

### System Requirements Created
- SYS-FUNC-XXX.Y: [Brief description]
- SYS-PERF-XXX.Y: [Brief description]  
- SYS-INT-XXX.Y: [Brief description]

### Documents Updated
- [ ] System Requirements Specification (SRS-AES5-001)
- [ ] Requirements Traceability Matrix
- [ ] Architecture Constraints Document
- [ ] Test Planning Documents

---

## Quality Assurance

### Requirement Quality Checklist
- [ ] Requirements are complete (no TBDs)
- [ ] Requirements are unambiguous  
- [ ] Requirements are verifiable
- [ ] Requirements are traceable to stakeholder needs
- [ ] Requirements follow SRS format standards

### Stakeholder Validation
- [ ] Stakeholder reviewed session notes
- [ ] Stakeholder approved derived requirements  
- [ ] Conflicts resolved or escalated
- [ ] Next session scheduled if needed

---

## Session Metrics

**Preparation Time**: [Hours]  
**Session Duration**: [Hours]  
**Follow-up Time**: [Hours]  
**Requirements Derived**: [Number]  
**Issues Identified**: [Number]  
**Stakeholder Satisfaction**: [1-10 scale]

---

**Session Notes Control:**
- **Prepared by**: [Requirements Engineer Name]
- **Reviewed by**: [Stakeholder Name(s)]
- **Approved by**: [Project Manager]
- **Date Finalized**: [YYYY-MM-DD]
- **Distribution**: Project team, stakeholders, QA team