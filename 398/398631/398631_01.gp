\\ a(n) = Sum_{k=0..3} (-2)^k * binomial(3,k) * (n+3-k)! for n >= 3.
print("境界がだめ");
a(n) = my(v=[6, 2, 28]); if(n<2, v[n+1], sum(k=0, 3, (-2)^k*binomial(3, k)*(n+3-k)!));
for(n=0, 20, print1(a(n),", "));

a(n) = my(v=[6, 2, 28]); if(n<3, v[n+1], sum(k=0, 3, (-2)^k*binomial(3, k)*(n+3-k)!));
for(n=0, 20, print1(a(n),", "));
