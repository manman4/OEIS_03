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

def A318572(n)
  ary = []
  i = 2
  while ary.size < n
    pq = i.prime_division
    if pq.all?{|j| j[1] == 1}
      ary << i if A(i) != pq[-1][0]
    end
    i += 1
  end
  ary
end

n = 500
ary = A318572(n)
(1..n).each{|i|
  j = ary[i - 1]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
