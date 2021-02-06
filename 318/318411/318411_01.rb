require 'prime'

def A(n)
  s = 1
  flag = false
  while !flag
    s += 1
    flag = true
    (1..n - 1).each{|i|
      if i != ((i ** s) % n)
        flag = false
        break
      end
    }
  end
  s
end

n = 50
ary = []
i = 2
while ary.size < n - 1
  p ary << A(i) if i.prime_division.to_a.all?{|j| j[1] == 1}
  i += 1
end
(2..n).each{|i|
  print i
  print ' '
  puts ary[i - 2]
}
p ary
