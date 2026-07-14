define i32 @h(i32 %x, i32 %v) {
entry:
  %pre = add i32 %v, 100
  switch i32 %x, label %def [
    i32 1, label %caseA
    i32 2, label %caseB
  ]

caseA:
  %pa = phi i32 [ %pre, %entry ]
  br label %exit

caseB:
  %pb = phi i32 [ %pre, %entry ]
  br label %exit

def:
  %pd = phi i32 [ %pre, %entry ]
  br label %exit

exit:
  %r = phi i32 [ %pa, %caseA ], [ %pb, %caseB ], [ %pd, %def ]
  ret i32 %r
}
