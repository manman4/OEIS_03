import math

def prime_table(n):
  list = [True for _ in range(n + 1)]
  i = 2
  while i * i <= n:
    if list[i]:
      j = i + i
      while j <= n:
        list[j] = False
        j += i
    i += 1

  table = [i for i in range(n + 1) if list[i] and i >= 2]
  return table

def a304480(n):
   if n == 1: return 1
   if n == 2: return 3
   ps = prime_table(n+1)
   ps = ps[1:]
   B = [0.] * n
   C = [1] * n
   for p in ps:
      k = 0
      while 1:
         x = p**k*(p-1)
         if x>=n: break
         x2 = p**(k+1)*(p-1)
         m = p**(k+1)
         for target in range(x,n,x):
            if target % x2 == 0:
               continue
            B[target] = B[target] + math.log(m)
            C[target] = C[target] * m
         k += 1
   
   best = 0
   for target in range(1,n):
      powers2 = 0
      x = target
      while not x&1:
         x>>=1
         powers2 += 1
      t = 2**(powers2+2)
      v = C[target] * t
      t = B[target] + math.log(t)
      if t>best:
         bestv = v
         best = t

   return bestv+1

n = 100
for i in range(1, n):
   print([i, a304480(i)])