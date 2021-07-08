


gcc -o a.out socket.c
./a.out 192.168.0.107 5000 POST /api/users/login/test "{\"email\":\"admin@example.com\",\"password\":\"123456\"}" "Content-Type:application/json"