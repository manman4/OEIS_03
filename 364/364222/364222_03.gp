a(n) = if(n%3==0, n + 3 * a(n/3), n);
for(n=1, 57, print1(a(n),", ")) 