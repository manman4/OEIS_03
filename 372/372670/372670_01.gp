isok(n) = ispower(n*eulerphi(n), 5);
for(n=1, 16384000, if(isok(n), print1(n, ", ")))

