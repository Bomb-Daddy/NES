"""Source-provenance 41-province adjacency adapted as independent original-game data.
Reference: Google Drive 1UpVEg8V1c0dsuZtPRsEMFjQQePRFfWP1;
NES ROM physical offset 0x28014; 41 reciprocal records, 93 edges.
This is a DATA ADAPTATION, not a port/import of NES engine logic.
Do not infer strategic-art polygon geometry or original-game gameplay rules from it.
"""
GRAPH41 = {
 1:(2,), 2:(1,3,6), 3:(2,4,6,7), 4:(3,5,7),
 5:(4,7,10,11), 6:(2,3,7,8,9), 7:(3,4,5,6,9,10),
 8:(6,9,16), 9:(6,7,8,10,16,17), 10:(5,7,9,11,17,19),
 11:(5,10,12,19,20), 12:(11,13,20,29), 13:(12,14,29,30),
 14:(13,15), 15:(14,), 16:(8,9,17,18,24),
 17:(9,10,16,18,19,28), 18:(16,17,24,25,27,28),
 19:(10,11,17,20,21,28), 20:(11,12,19,21,29,31),
 21:(19,20,22,23,28,31), 22:(21,23,27,28,37,38),
 23:(21,22,31,38,39,40), 24:(16,18,25),
 25:(18,24,26,27), 26:(25,27,37),
 27:(18,22,25,26,28,37), 28:(17,18,19,21,22,27),
 29:(12,13,20,30,31,32), 30:(13,29,32,33),
 31:(20,21,23,29,32,40), 32:(29,30,31,33,34,40),
 33:(30,32,34,35), 34:(32,33,35,36,40,41),
 35:(33,34,36), 36:(34,35,41), 37:(22,26,27,38),
 38:(22,23,37,39), 39:(23,38,40,41),
 40:(23,31,32,34,39,41), 41:(34,36,39,40),
}

def validate_graph41(graph=GRAPH41):
    if set(graph) != set(range(1,42)):
        raise ValueError('Province IDs must be exactly 1..41')
    for province, adjacent in graph.items():
        if len(adjacent) != len(set(adjacent)) or province in adjacent:
            raise ValueError('Duplicate or self-edge')
        for neighbor in adjacent:
            if neighbor not in graph or province not in graph[neighbor]:
                raise ValueError('Missing reciprocal edge')
    edge_count=sum(len(v) for v in graph.values())//2
    if edge_count != 93:
        raise ValueError('Expected 93 reciprocal undirected edges, got '+str(edge_count))
    visited={1}
    while True:
        next_seen=visited | {n for p in visited for n in graph[p]}
        if next_seen==visited: break
        visited=next_seen
    if len(visited)!=41:raise ValueError('Graph is disconnected')
    if set(graph[4]) != {3,5,7} or set(graph[40]) != {23,31,32,34,39,41}:
        raise ValueError('Province 4/40 mixup')
    return True

def serialize_neighbors(graph=GRAPH41):
    validate_graph41(graph)
    return {f'p{k}':[f'p{v}' for v in values] for k,values in graph.items()}
