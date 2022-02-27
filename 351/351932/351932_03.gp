M=28;

a(n) = if(n<4, 1, a(n-1)+binomial(n-1, 3)*a(n-4));
for(n=0, M, print1(a(n), ", "));