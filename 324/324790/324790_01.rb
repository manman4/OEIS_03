require 'prime'

@ary = Prime.take(7 * 10 ** 7).to_a

def A(n)
   s = 1
   a = [1]
   (1..n).each{|i|
     j = @ary[i - 1]
     if s < j
       s += j
     else
       s -= j
     end
     a << s
     p i if s == 0
   }
   a
end

n = 7 * 10 ** 7
ary = A(n)
