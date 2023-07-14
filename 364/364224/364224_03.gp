a(n) = if(n%7==0, n + 7 * a(n/7), n);
for(n=1, 57, print1(a(n),", ")) 