def avoids_132?(perm)
  third = 0
  stack = []
  i = perm.length - 1

  while i >= 0
    x = perm[i]
    return false if x < third

    while !stack.empty? && stack[-1] < x
      third = stack.pop
    end
    stack << x
    i -= 1
  end
  true
end

def order_divides_3?(perm)
  n = perm.length
  used = Array.new(n, false)

  0.upto(n - 1){|start|
    next if used[start]

    len = 0
    i = start
    until used[i]
      used[i] = true
      i = perm[i] - 1
      len += 1
      return false if len > 3
    end
    return false unless len == 1 || len == 3
  }
  true
end

def build_templates(limit)
  all_templates = Array.new(limit + 1){[]}
  tail_templates = Array.new(limit + 1){[]}

  3.upto(limit){|n|
    all = []
    tail = []

    2.upto(n - 1){|s2|
      (s2 + 1).upto(n){|s3|
        keep = []
        1.upto(n){|v|
          next if v == 1 || v == s2 || v == s3
          keep << v
        }

        item0 = [keep, s2, s3, 0]
        item1 = [keep, s2, s3, 1]
        all << item0 << item1
        if s3 == n
          tail << item0 << item1
        end
      }
    }

    all_templates[n] = all
    tail_templates[n] = tail
  }

  [all_templates, tail_templates]
end

def fill_child!(scratch, perm, keep, s2, s3, orient)
  0.upto(perm.length - 1){|i|
    scratch[keep[i] - 1] = keep[perm[i] - 1]
  }

  if orient == 0
    scratch[0] = s2
    scratch[s2 - 1] = s3
    scratch[s3 - 1] = 1
  else
    scratch[0] = s3
    scratch[s3 - 1] = s2
    scratch[s2 - 1] = 1
  end
end

def generate_good_perms(limit, verify: false)
  raise ArgumentError, "limit must be nonnegative" if limit < 0

  all_templates, tail_templates = build_templates(limit)
  good = Array.new(limit + 1){[]}
  good[0] = [[]]

  1.upto(limit){|n|
    list = []

    good[n - 1].each{|perm|
      child = perm + [n]
      if verify
        raise "132 check failed" unless avoids_132?(child)
        raise "order check failed" unless order_divides_3?(child)
      end
      list << child
    }

    if n < 3
      good[n] = list
      next
    end

    scratch = Array.new(n, 0)
    good[n - 3].each{|perm|
      # Canonical generation:
      # if the parent already ends with a fixed point, then any 3-cycle insertion
      # with s3 < n would create a child also ending with a fixed point, and that
      # child is generated uniquely by the append step instead.
      templates = perm[-1] == perm.length ? tail_templates[n] : all_templates[n]

      templates.each{|keep, s2, s3, orient|
        fill_child!(scratch, perm, keep, s2, s3, orient)
        next unless avoids_132?(scratch)

        child = scratch.dup
        if verify
          raise "order check failed" unless order_divides_3?(child)
        end
        list << child
      }
    }

    good[n] = list
  }

  good
end

def sequence(limit, verify: false)
  generate_good_perms(limit, verify: verify).map(&:length)
end

if __FILE__ == $PROGRAM_NAME
  limit = (ARGV[0] || 20).to_i
  verify = ARGV.include?("--verify")
  ary = sequence(limit, verify: verify)
  puts ary.join(", ")
  ary.each_with_index{|v, n|
    puts "#{n} #{v}"
  }
end
