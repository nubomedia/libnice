
#include "agent.h"


/* (ICE-13 §4.1.1) Every candidate is a transport address. It also has a type and
 * a base. Three types are defined and gathered by this specification - host
 * candidates, server reflexive candidates, and relayed candidates. */


Candidate *
candidate_new (CandidateType type)
{
  Candidate *candidate;

  candidate = g_slice_new0 (Candidate);
  candidate->type = type;
  return candidate;
}


void
candidate_free (Candidate *candidate)
{
  g_slice_free (Candidate, candidate);
}


gfloat
candidate_jingle_priority (Candidate *candidate)
{
  switch (candidate->type)
    {
    case CANDIDATE_TYPE_HOST:             return 1.0;
    case CANDIDATE_TYPE_SERVER_REFLEXIVE: return 0.9;
    case CANDIDATE_TYPE_PEER_REFLEXIVE:   return 0.9;
    case CANDIDATE_TYPE_RELAYED:          return 0.5;
    }

  /* appease GCC */
  return 0;
}


/* ICE-13 §4.1.2; returns number between 1 and 0x7effffff */
G_GNUC_CONST
guint32
_candidate_ice_priority (
  // must be ∈ (0, 126) (max 2^7 - 2)
  guint type_preference,
  // must be ∈ (0, 65535) (max 2^16 - 1)
  guint local_preference,
  // must be ∈ (1, 255) (max 2 ^ 8 - 1)
  guint component_id)
{
  return (
      0x1000000 * type_preference +
      0x100 * local_preference +
      (0x100 - component_id));
}


guint32
candidate_ice_priority (Candidate *candidate)
{
  guint8 type_preference = 0;

  switch (candidate->type)
    {
    case CANDIDATE_TYPE_HOST:             type_preference = 120; break;
    case CANDIDATE_TYPE_PEER_REFLEXIVE:   type_preference = 110; break;
    case CANDIDATE_TYPE_SERVER_REFLEXIVE: type_preference = 100; break;
    case CANDIDATE_TYPE_RELAYED:          type_preference =  60; break;
    }

  return _candidate_ice_priority (type_preference, 1, candidate->component_id);
}

