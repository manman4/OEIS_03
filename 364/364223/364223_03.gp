a(n) = if(n%5==0, n + 5 * a(n/5), n);
for(n=1, 57, print1(a(n),", ")) 