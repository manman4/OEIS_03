require 'prime'

def A(n)
  (1..n).map{|i| (2 * i - 1) % 10}.join.to_i
end

ary = []
(1..25).each{|i|
  j = A(i)
  if j.prime?
    ary << j
    p j
  end
}
p ary
