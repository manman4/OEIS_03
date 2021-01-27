require 'prime'

def A(n)
   s = 3
   pr = 3
   ary = []
   (2..n).each{|i|
p [i, ary]
     while ! pr.prime?
       pr += 2
     end
     if s < pr
       s += pr
     else
       s -= pr
     end
     pr += 2
     ary << i if s == 0
   }
  ary
end

n = 10 ** 9
p A(n)
