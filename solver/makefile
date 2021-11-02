all: card_test foundation_test tableau_test bit_stream_test \
	   node_test solver visualizer

card_test: card_test.cc card.h
	g++ -g -std=c++0x -o $@ $(filter %.cc,$^)
	./$@

foundation_test: foundation_test.cc foundation.h card.h
	g++ -g -std=c++0x -o $@ $(filter %.cc,$^)
	./$@

tableau_test: tableau_test.cc tableau.cc tableau.h card.h
	g++ -g -std=c++0x -o $@ $(filter %.cc,$^)
	./$@

bit_stream_test: bit_stream_test.cc bit_stream.h
	g++ -g -std=c++0x -o $@ $(filter %.cc,$^)
	./$@

NODE_SRC=node.cc node.h list.h move.cc move.h options.cc options.h \
				 tableau.cc tableau.h foundation.h array.h card.h bit_stream.h

node_test: node_test.cc $(NODE_SRC)
	g++ -g -std=c++0x -o $@ $(filter %.cc,$^)
	./$@

SOLVER_SRC=solver.cc $(NODE_SRC) deals.cc deals.h bucket.h hash_table.h

solver: $(SOLVER_SRC)
	g++ -O3 -std=c++0x -o $@ $(filter %.cc,$^) -pthread

solver.g: $(SOLVER_SRC)
	g++ -g -std=c++0x -o $@ $(filter %.cc,$^) -pthread -DDEBUG

solver.p: $(SOLVER_SRC)
	g++ -O3 -std=c++0x -fprofile-generate -o $@ $(filter %.cc,$^) -pthread
	time ./$@ 12 12

solver.q: $(SOLVER_SRC)
	g++ -O3 -std=c++0x -fprofile-use -o $@ $(filter %.cc,$^) -pthread
	time ./$@ 12 12

visualizer: visualizer.cc $(NODE_SRC) deals.cc deals.h
	g++ -O3 -std=c++0x -o $@ $(filter %.cc,$^)

solver.js: $(SOLVER_SRC)
	emcc -O3 -std=c++17 -o $@ $(filter %.cc,$^) \
		-s ALLOW_MEMORY_GROWTH \
		-s ASSERTIONS=1 \
		-s EXPORTED_FUNCTIONS='["_solve"]' \
		-s EXPORTED_RUNTIME_METHODS='["ccall","cwrap"]'
