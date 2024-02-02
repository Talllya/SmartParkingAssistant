
	gcc ./src/utils.c ./src/availableSpots.c ./src/baza.c ./src/cam_senz.c -o ./dist/cam -lsqlite3
	gcc ./src/utils.c ./src/availableSpots.c ./src/baza.c ./src/server.c -o ./dist/server -lsqlite3
	gcc ./src/client.c -o ./dist/client
	
	./dist/server

