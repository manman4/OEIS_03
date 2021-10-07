require 'prime'

def A(n)
  s, t = 0, 0
  (n ** 2..(n + 1) ** 2).each{|i|
    if i.prime?
      s += 1 if i % 4 == 3
      t += 1 if i % 4 == 1
    end
  }
  s - t
end

n = 10000
(1..n).each{|i|
  print i
  print ' '
  puts A(i)
}
