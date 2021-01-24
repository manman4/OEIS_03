def A(n)
  a = 0
  ary = [0]
  used = Array.new(10 ** 8, false)
  (1..n).each{|i|
    j = a - i
    if j > 0 && !used[j]
      a = j
    else
      a += i
    end
    ary << a
    # 仮にaがn以上でもusedのサイズが大きくなるので大丈夫
    used[a] = true
  }
  ary
end

n = 10 ** 4
ary = A(n)
(0..n).each{|i| 
  print i
  print " "
  puts ary[0..i].select{|j| j > i}.size
}