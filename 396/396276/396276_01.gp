FussCatalan_B(steps) = { 
  my(x = 'x, P = 1, x_k = x); 
  for(k = 0, steps - 1, P = P * (1 + x_k); x_k = x * P^2); 
  return(P); 
}

for(s = 0, 5, print(FussCatalan_B(s) + x*O(x^((3^s-1)/2))));
for(s = 0, 5, print(Vec(FussCatalan_B(s) + x*O(x^((3^s-1)/2)))));


FussCatalan_B(steps) = { 
  my(x = 'x, P = 1, x_k = x); 
  for(k = 0, steps - 1, P = P * (1 + x_k); x_k = x * P^3); 
  return(P); 
}


for(s = 0, 4, print(FussCatalan_B(s) + x*O(x^((4^s-1)/3))));
for(s = 0, 4, print(Vec(FussCatalan_B(s) + x*O(x^((4^s-1)/3)))));
