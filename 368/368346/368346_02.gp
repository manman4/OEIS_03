\\ a(n) = a(n-4) + 2^(n-3) - 1.
a(n) = if(n<4, 0, a(n-4) + 2^(n-3) - 1);
for(n=0, 20, print1(a(n), ", "));

 