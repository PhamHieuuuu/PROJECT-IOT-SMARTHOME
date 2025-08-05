package com.example.iothome

import android.content.Intent
import android.net.Uri
import androidx.compose.foundation.background
import androidx.compose.foundation.clickable
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material3.*
import androidx.compose.material.Card
import androidx.compose.foundation.BorderStroke
import androidx.compose.foundation.Image
import androidx.compose.ui.res.painterResource
import androidx.compose.ui.platform.LocalContext
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp

@Composable
fun MainScreen(viewModel: MainViewModel) {
    val context = LocalContext.current  // ✅ Khai báo context

    Column(
        modifier = Modifier
            .fillMaxSize()
            .padding(16.dp),
        horizontalAlignment = Alignment.CenterHorizontally
    ) {
        Spacer(modifier = Modifier.height(30.dp)) // Dịch xuống
        Text(
            text = "              Hệ Thống IOT\nĐiều Khiển Relay & Cảm Biến",
            fontSize = 22.sp,
            fontWeight = FontWeight.Bold
        )

        Spacer(modifier = Modifier.height(25.dp))

        Card(
            shape = RoundedCornerShape(12.dp),
            elevation = 6.dp,
            modifier = Modifier.padding(8.dp)
        ) {
            Column(
                modifier = Modifier
                    .padding(16.dp)
                    .fillMaxWidth(),
                horizontalAlignment = Alignment.CenterHorizontally
            ) {
                Text(text = "Nhiệt Độ: ${viewModel.temp.value} °C", fontSize = 18.sp)
                Text(text = "Độ Ẩm: ${viewModel.humidity.value} %", fontSize = 18.sp)
                Text(
                    text = "Khí Gas: ${if (viewModel.gas.value == "0") "Không Phát Hiện" else "Phát Hiện Khí Gas!"}",
                    fontSize = 18.sp,
                    color = if (viewModel.gas.value == "0") Color.Black else Color.Red
                )
            }
        }

        Spacer(modifier = Modifier.height(24.dp)) // Dịch xuống

        Card(
            shape = RoundedCornerShape(12.dp),
            elevation = 6.dp,
            modifier = Modifier.padding(8.dp)
        ) {
            Column(
                modifier = Modifier
                    .padding(16.dp)
                    .fillMaxWidth(),
                horizontalAlignment = Alignment.CenterHorizontally
            ) {
                for (i in 0 until 6 step 2) {
                    Row(
                        modifier = Modifier.fillMaxWidth(),
                        horizontalArrangement = Arrangement.SpaceEvenly
                    ) {
                        RelayButton(relayState = viewModel.relayStates[i], onClick = { viewModel.toggleRelay(i) }, relayNumber = i + 1)
                        RelayButton(relayState = viewModel.relayStates[i + 1], onClick = { viewModel.toggleRelay(i + 1) }, relayNumber = i + 2)
                    }
                }
                Spacer(modifier = Modifier.height(8.dp))
                RelayButton(relayState = viewModel.doorState.value, onClick = { viewModel.toggleDoor() }, relayNumber = 7, label = "Cửa")
            }
        }

        Spacer(modifier = Modifier.height(24.dp))

        Card(
            shape = RoundedCornerShape(12.dp),
            elevation = 6.dp,
            modifier = Modifier.padding(8.dp)
        ) {
            Column(
                modifier = Modifier
                    .padding(16.dp)
                    .fillMaxWidth(),
                horizontalAlignment = Alignment.CenterHorizontally
            ) {
                Text(text = "Sản phẩm được thiết kế bởi", fontWeight = FontWeight.Bold, fontSize = 16.sp)
                Text(text = "Phạm Văn Hiếu", fontSize = 14.sp, fontWeight = FontWeight.Medium)
                Text(text = "-- Liên hệ với tôi qua --", fontSize = 12.sp, fontStyle = androidx.compose.ui.text.font.FontStyle.Italic)

                Row(
                    modifier = Modifier.fillMaxWidth(),
                    horizontalArrangement = Arrangement.Center
                ) {
                    val facebookUrl = "https://www.facebook.com/share/1XXaWbbq83/"
                    val youtubeUrl = "https://www.youtube.com/channel/UC4_lJFNG01mosrWqmJNaEpQ"
                    val profileUrl = "https://www.phamhieublog.site/"

                    Image(
                        painter = painterResource(id = R.drawable.facebook),
                        contentDescription = "Facebook",
                        modifier = Modifier
                            .size(40.dp)
                            .clickable {
                                val intent = Intent(Intent.ACTION_VIEW, Uri.parse(facebookUrl))
                                context.startActivity(intent) // ✅ Mở link Facebook
                            }
                    )

                    Spacer(modifier = Modifier.width(16.dp))

                    Image(
                        painter = painterResource(id = R.drawable.youtube),
                        contentDescription = "YouTube",
                        modifier = Modifier
                            .size(40.dp)
                            .clickable {
                                val intent = Intent(Intent.ACTION_VIEW, Uri.parse(youtubeUrl))
                                context.startActivity(intent) // ✅ Mở link YouTube
                            }
                    )

                    Spacer(modifier = Modifier.width(16.dp))

                    Image(
                        painter = painterResource(id = R.drawable.logo),
                        contentDescription = "Profile",
                        modifier = Modifier
                            .size(40.dp)
                            .clickable {
                                val intent = Intent(Intent.ACTION_VIEW, Uri.parse(profileUrl))
                                context.startActivity(intent) // ✅ Mở link Profile
                            }
                    )
                }
            }
        }
    }
}

@Composable
fun RelayButton(relayState: Boolean, onClick: () -> Unit, relayNumber: Int, label: String = "Relay $relayNumber") {
    Column(horizontalAlignment = Alignment.CenterHorizontally) {
        Text(
            text = if (relayState) "ON" else "OFF",
            color = if (relayState) Color(0xFF28A745) else Color(0xFFFF0000), // Xanh lá đậm & Đỏ
            fontWeight = FontWeight.Bold
        )
        Button(
            onClick = onClick,
            modifier = Modifier
                .padding(8.dp)
                .width(120.dp)
                .height(50.dp),
            colors = ButtonDefaults.buttonColors(
                containerColor = if (relayState) Color(0xFF28A745) else Color.White, // Xanh lá hoặc trắng
                contentColor = if (relayState) Color.White else Color.Black
            ),
            shape = RoundedCornerShape(12.dp),
            border = BorderStroke(1.dp, if (relayState) Color(0xFF28A745) else Color.Black) // Viền xanh lá khi ON, đen khi OFF
        ) {
            Text(text = label, fontSize = 16.sp, fontWeight = FontWeight.Bold)
        }
    }
}
