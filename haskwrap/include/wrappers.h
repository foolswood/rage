#pragma once
#include "loader.h"
#include "graph.h"

rage_ElementTypeLoadResult * rage_element_loader_load_hs(rage_ElementLoader * l, char * name);
rage_NewConcreteElementType * rage_element_type_specialise_hs(rage_ElementType * t, rage_Atom ** params);
rage_NewGraph * rage_graph_new_hs(uint32_t sample_rate);
rage_NewGraphNode * rage_graph_add_node_hs(
    rage_Graph * g, rage_ConcreteElementType * cet, char const * name,
    rage_TimeSeries const * ts);