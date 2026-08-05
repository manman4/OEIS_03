\\ a(n) = Sum_{k=0..5} (-2)^k * binomial(5,k) * (n+5-k)! for n >= 5.
print("境界がだめ");
a(n) = my(v=[120, 90, 952, 9936, 108576]); if(n<4, v[n+1], sum(k=0, 5, (-2)^k*binomial(5, k)*(n+5-k)!));
for(n=0, 18, print1(a(n),", "));

a(n) = my(v=[120, 90, 952, 9936, 108576]); if(n<5, v[n+1], sum(k=0, 5, (-2)^k*binomial(5, k)*(n+5-k)!));
for(n=0, 18, print1(a(n),", "));
