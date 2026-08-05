\\ a(n) = Sum_{k=0..2} (-2)^k * binomial(2,k) * (n+2-k)! for n >= 2.
print("境界がだめ");
a(n) = my(v=[2, 0]); if(n<1, v[n+1], sum(k=0, 2, (-2)^k*binomial(2, k)*(n+2-k)!));
for(n=0, 21, print1(a(n),", "));

a(n) = my(v=[2, 0]); if(n<2, v[n+1], sum(k=0, 2, (-2)^k*binomial(2, k)*(n+2-k)!));
for(n=0, 21, print1(a(n),", "));
