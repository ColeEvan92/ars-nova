Eliminate a lot of winapi boilerplate code. Also prevents Windows from disappearing into the DefWindowProc move/size loop that eats most messages. Message pump exists in another thread.
