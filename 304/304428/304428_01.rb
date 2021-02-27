# ˜aˆöŽq‚ÍminˆÈãmaxˆÈ‰º
def partition(n, min, max)
  return [[]] if n == 0
  [max, n].min.downto(min).flat_map{|i| partition(n - i, min, i).map{|rest| [i, *rest]}}
end

print 0
print ' '
puts 1
(1..70).each{|i|
  cnt = 0
  partition(i, 1, i).each{|ary|
    a = ary.group_by{|i| i}.sort
    m = a[0][1].inject(:+)
    flag = true
    if a.size > 1
      (a[1..-1]).each{|b|
        mb = b[1].inject(:+)
        if m < mb
          m = mb
        else
          flag = false
        end
      }
    end
    if flag
      cnt += 1
    end
  }
  print i
  print ' '
  puts cnt
}
