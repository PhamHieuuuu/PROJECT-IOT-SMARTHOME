package com.example.iothome  // Đảm bảo giống với MainActivity.kt

import androidx.lifecycle.ViewModel
import androidx.compose.runtime.mutableStateListOf
import androidx.compose.runtime.mutableStateOf
import com.google.firebase.database.*

class MainViewModel : ViewModel() {
    private val db: DatabaseReference = FirebaseDatabase.getInstance().reference

    var temp = mutableStateOf("0.0")
    var humidity = mutableStateOf("0")
    var gas = mutableStateOf("0")

    var relayStates = mutableStateListOf(false, false, false, false, false, false)

    init {

        db.child("/NhietDo").addValueEventListener(object : ValueEventListener {
            override fun onDataChange(snapshot: DataSnapshot) {
                temp.value = snapshot.getValue(Double::class.java)?.toString() ?: "0.0"
            }
            override fun onCancelled(error: DatabaseError) {}
        })

        db.child("/DoAm").addValueEventListener(object : ValueEventListener {
            override fun onDataChange(snapshot: DataSnapshot) {
                humidity.value = snapshot.getValue(Int::class.java)?.toString() ?: "0"
            }
            override fun onCancelled(error: DatabaseError) {}
        })

        db.child("/Gas").addValueEventListener(object : ValueEventListener {
            override fun onDataChange(snapshot: DataSnapshot) {
                gas.value = snapshot.getValue(Int::class.java)?.toString() ?: "0"
            }
            override fun onCancelled(error: DatabaseError) {}
        })

        for (i in 1..6) {
            db.child("/button$i").addValueEventListener(object : ValueEventListener {
                override fun onDataChange(snapshot: DataSnapshot) {
                    relayStates[i - 1] = snapshot.getValue(Int::class.java) == 1
                }
                override fun onCancelled(error: DatabaseError) {}
            })
        }
    }

    fun toggleRelay(index: Int) {
        val newState = !relayStates[index]
        relayStates[index] = newState
        db.child("/button${index + 1}").setValue(if (newState) 1 else 0)
    }

    var doorState = mutableStateOf(false)
        private set  // Đảm bảo chỉ thay đổi trong ViewModel
    fun toggleDoor() {
        doorState.value = true // Đổi màu sang ON khi nhấn
        db.child("/C").setValue(1) // Gửi dữ liệu lên Firebase

        // Sau 2 giây, tự động đặt lại OFF
        Thread {
            Thread.sleep(2000)
            db.child("/C").setValue(0)
            doorState.value = false // Quay về OFF
        }.start()
    }



}
